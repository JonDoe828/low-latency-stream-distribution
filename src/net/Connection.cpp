#include "net/Connection.h"

namespace media_relay {

Connection::Connection(Socket socket) : socket_(std::move(socket)) {}

auto Connection::fd() const -> int {
    return socket_.fd();
}

auto Connection::inputBuffer() -> Buffer& {
    return input_buffer_;
}

auto Connection::outputBuffer() -> Buffer& {
    return output_buffer_;
}

auto Connection::send(const std::vector<std::uint8_t>& bytes) -> bool {
    output_buffer_.append(bytes);
    while (!output_buffer_.empty()) {
        const auto written = output_buffer_.writeFd(socket_.fd());
        if (written <= 0) {
            return false;
        }
    }
    return true;
}

void Connection::close() {
    socket_.close();
}

}  // namespace media_relay
