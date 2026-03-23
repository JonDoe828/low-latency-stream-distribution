#include <cstdint>
#include <iostream>
#include <string>

#include <sys/socket.h>
#include <unistd.h>

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
        std::cerr << "usage: subscriber_client <host> <port> <stream_id>\n";
        return 1;
    }

    const std::string host = argv[1];
    const auto port = static_cast<std::uint16_t>(std::stoi(argv[2]));
    const std::string stream_id = argv[3];

    auto socket = media_relay::Socket::createTcp();
    if (!socket.valid() || !socket.connect(host, port)) {
        std::cerr << "failed to connect to relay server\n";
        return 1;
    }

    if (!sendMessage(socket.fd(), media_relay::Message::makeSubscribe(stream_id))) {
        std::cerr << "failed to send subscribe command\n";
        return 1;
    }

    media_relay::Buffer buffer;
    media_relay::Message response;
    if (!readMessage(socket.fd(), buffer, response)) {
        std::cerr << "subscribe rejected: failed to read server response\n";
        return 1;
    }

    if (response.type == media_relay::CommandType::Error) {
        std::cerr << "subscribe rejected: " << response.text << '\n';
        return 1;
    }

    std::cout << "subscribed to stream " << stream_id << '\n';
    while (readMessage(socket.fd(), buffer, response)) {
        if (response.type != media_relay::CommandType::Frame) {
            continue;
        }

        const std::string payload(response.payload.begin(), response.payload.end());
        std::cout << "frame seq=" << response.sequence_number
                  << " ts=" << response.timestamp_us
                  << " payload=\"" << payload << "\"\n";
    }

    std::cerr << "stream closed\n";
    return 0;
}
