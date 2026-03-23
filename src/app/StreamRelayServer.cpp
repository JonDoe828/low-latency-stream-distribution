#include "app/StreamRelayServer.h"

#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <string>
#include <thread>

#include "base/Logger.h"
#include "base/Timestamp.h"
#include "net/Acceptor.h"
#include "net/Buffer.h"
#include "protocol/Codec.h"

namespace media_relay {
namespace {

struct SubscriberQueue {
    std::mutex mutex;
    std::condition_variable condition;
    std::deque<StreamFrame> frames;
    bool active {true};
};

auto sendAll(int fd, const std::vector<std::uint8_t>& bytes) -> bool {
    std::size_t sent = 0;
    while (sent < bytes.size()) {
        const auto result = ::send(
            fd,
            reinterpret_cast<const char*>(bytes.data() + sent),
            bytes.size() - sent,
            MSG_NOSIGNAL);
        if (result <= 0) {
            return false;
        }
        sent += static_cast<std::size_t>(result);
    }
    return true;
}

auto sendMessage(int fd, const Message& message) -> bool {
    return sendAll(fd, Codec::encode(message));
}

auto readMessage(int fd, Buffer& buffer, Message& message) -> bool {
    while (true) {
        std::string error_message;
        const auto status = Codec::tryDecode(buffer, message, &error_message);
        if (status == Codec::DecodeStatus::Ok) {
            return true;
        }

        if (status == Codec::DecodeStatus::Error) {
            Logger::warn("closing malformed connection: " + error_message);
            return false;
        }

        const auto bytes_read = buffer.readFd(fd);
        if (bytes_read <= 0) {
            return false;
        }
    }
}

}  // namespace

StreamRelayServer::StreamRelayServer(Config config)
    : config_(std::move(config)),
      backpressure_policy_(
          config_.max_subscriber_queue,
          config_.disconnect_slow_subscriber) {}

StreamRelayServer::~StreamRelayServer() {
    stop();
    wait();
}

auto StreamRelayServer::start() -> bool {
    listen_socket_ = Socket::createTcp();
    if (!listen_socket_.valid()) {
        return false;
    }

    listen_socket_.setReuseAddr(true);
    if (!listen_socket_.bind(config_.host, config_.port) ||
        !listen_socket_.listen(config_.backlog)) {
        listen_socket_.close();
        return false;
    }

    running_.store(true);
    accept_thread_ = std::thread(&StreamRelayServer::acceptLoop, this);
    Logger::info(
        "stream relay listening on " + config_.host + ":" + std::to_string(config_.port));
    return true;
}

void StreamRelayServer::stop() {
    if (!running_.exchange(false)) {
        return;
    }

    if (listen_socket_.valid()) {
        ::shutdown(listen_socket_.fd(), SHUT_RDWR);
        listen_socket_.close();
    }
}

void StreamRelayServer::wait() {
    if (accept_thread_.joinable()) {
        accept_thread_.join();
    }
}

void StreamRelayServer::acceptLoop() {
    while (running_.load()) {
        const int client_fd = ::accept(listen_socket_.fd(), nullptr, nullptr);
        if (client_fd < 0) {
            if (running_.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
            continue;
        }

        const auto connection_id =
            "conn-" + std::to_string(next_connection_id_.fetch_add(1));
        std::thread(&StreamRelayServer::handleClient, this, client_fd, connection_id).detach();
    }
}

void StreamRelayServer::handleClient(int client_fd, std::string connection_id) {
    Buffer buffer;
    Message first_message;
    if (!readMessage(client_fd, buffer, first_message)) {
        ::close(client_fd);
        return;
    }

    if (first_message.type == CommandType::Publish) {
        if (!stream_manager_.registerPublisher(first_message.stream_id, connection_id)) {
            sendMessage(
                client_fd,
                Message::makeError("publisher already exists", first_message.stream_id));
            ::close(client_fd);
            return;
        }

        sendMessage(client_fd, Message::makeAck("publisher registered"));
        Logger::info(
            connection_id + " publishing stream " + first_message.stream_id);

        Message message;
        while (running_.load() && readMessage(client_fd, buffer, message)) {
            if (message.type == CommandType::Frame) {
                if (message.stream_id.empty()) {
                    message.stream_id = first_message.stream_id;
                }
                if (message.stream_id != first_message.stream_id) {
                    sendMessage(
                        client_fd,
                        Message::makeError("stream id mismatch", first_message.stream_id));
                    break;
                }
                const auto publish_result =
                    stream_manager_.publishFrame(StreamFrame::fromMessage(message));
                if (publish_result.dropped > 0) {
                    Logger::warn(
                        "dropped delivery to " + std::to_string(publish_result.dropped) +
                        " subscriber(s) on stream " + first_message.stream_id);
                }
            } else if (message.type == CommandType::Heartbeat) {
                sendMessage(client_fd, Message::makeAck("heartbeat"));
            } else {
                sendMessage(
                    client_fd,
                    Message::makeError("unexpected command for publisher", first_message.stream_id));
                break;
            }
        }

        stream_manager_.unregisterPublisher(first_message.stream_id);
        Logger::info(
            connection_id + " disconnected from publisher stream " + first_message.stream_id);
        ::close(client_fd);
        return;
    }

    if (first_message.type == CommandType::Subscribe) {
        auto queue = std::make_shared<SubscriberQueue>();
        auto subscriber = std::make_shared<Subscriber>(
            connection_id,
            [queue, this](const StreamFrame& frame) {
                std::lock_guard<std::mutex> lock(queue->mutex);
                if (!queue->active) {
                    return false;
                }

                const auto action = backpressure_policy_.pushFrame(queue->frames, frame);
                if (action == OverflowAction::Disconnect) {
                    queue->active = false;
                    queue->condition.notify_all();
                    return false;
                }

                queue->condition.notify_one();
                return true;
            });

        stream_manager_.addSubscriber(first_message.stream_id, subscriber);
        sendMessage(client_fd, Message::makeAck("subscriber registered"));
        Logger::info(
            connection_id + " subscribed to stream " + first_message.stream_id);

        while (running_.load()) {
            StreamFrame frame;
            {
                std::unique_lock<std::mutex> lock(queue->mutex);
                queue->condition.wait(lock, [&queue, this] {
                    return !queue->active || !queue->frames.empty() || !running_.load();
                });

                if (!queue->active || !running_.load()) {
                    break;
                }

                frame = std::move(queue->frames.front());
                queue->frames.pop_front();
            }

            if (!sendMessage(client_fd, frame.toMessage())) {
                break;
            }
        }

        {
            std::lock_guard<std::mutex> lock(queue->mutex);
            queue->active = false;
            queue->condition.notify_all();
        }
        stream_manager_.removeSubscriber(first_message.stream_id, connection_id);
        Logger::info(
            connection_id + " unsubscribed from stream " + first_message.stream_id);
        ::close(client_fd);
        return;
    }

    sendMessage(client_fd, Message::makeError("first command must be publish or subscribe"));
    ::close(client_fd);
}

}  // namespace media_relay
