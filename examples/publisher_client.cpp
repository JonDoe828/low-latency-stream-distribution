#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <sys/socket.h>
#include <unistd.h>

#include "base/Timestamp.h"
#include "net/Buffer.h"
#include "net/Socket.h"
#include "protocol/Codec.h"

namespace {

auto readMessage(int fd, media_relay::Buffer& buffer, media_relay::Message& message) -> bool {
    while (true) {
        const auto status = media_relay::Codec::tryDecode(buffer, message);
        if (status == media_relay::Codec::DecodeStatus::Ok) {
            return true;
        }
        if (status == media_relay::Codec::DecodeStatus::Error) {
            return false;
        }
        if (buffer.readFd(fd) <= 0) {
            return false;
        }
    }
}

auto sendMessage(int fd, const media_relay::Message& message) -> bool {
    const auto bytes = media_relay::Codec::encode(message);
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

}  // namespace

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "usage: publisher_client <host> <port> <stream_id> [frame_count]\n";
        return 1;
    }

    const std::string host = argv[1];
    const auto port = static_cast<std::uint16_t>(std::stoi(argv[2]));
    const std::string stream_id = argv[3];
    const int frame_count = argc >= 5 ? std::stoi(argv[4]) : 10;

    auto socket = media_relay::Socket::createTcp();
    if (!socket.valid() || !socket.connect(host, port)) {
        std::cerr << "failed to connect to relay server\n";
        return 1;
    }

    if (!sendMessage(socket.fd(), media_relay::Message::makePublish(stream_id, "text/plain"))) {
        std::cerr << "failed to send publish command\n";
        return 1;
    }

    media_relay::Buffer buffer;
    media_relay::Message response;
    if (!readMessage(socket.fd(), buffer, response)) {
        std::cerr << "publish rejected: failed to read server response\n";
        return 1;
    }

    if (response.type == media_relay::CommandType::Error) {
        std::cerr << "publish rejected: " << response.text << '\n';
        return 1;
    }

    for (int index = 0; index < frame_count; ++index) {
        const std::string payload_text = "frame-" + std::to_string(index);
        std::vector<std::uint8_t> payload(payload_text.begin(), payload_text.end());
        const auto timestamp_us =
            static_cast<std::uint64_t>(media_relay::Timestamp::now().microseconds());

        if (!sendMessage(
                socket.fd(),
                media_relay::Message::makeFrame(
                    stream_id,
                    static_cast<std::uint64_t>(index + 1),
                    std::move(payload),
                    timestamp_us))) {
            std::cerr << "failed to send frame " << index << '\n';
            return 1;
        }

        std::cout << "published frame " << index + 1 << '\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    return 0;
}
