#include "net/Buffer.h"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <system_error>

#include <sys/socket.h>
#include <unistd.h>

namespace media_relay {

auto Buffer::readableBytes() const -> std::size_t {
    return buffer_.size() - read_index_;
}

auto Buffer::empty() const -> bool {
    return readableBytes() == 0;
}

auto Buffer::peek() const -> const std::uint8_t* {
    return buffer_.data() + read_index_;
}

void Buffer::append(const void* data, std::size_t length) {
    const auto* bytes = static_cast<const std::uint8_t*>(data);
    buffer_.insert(buffer_.end(), bytes, bytes + length);
}

void Buffer::append(const std::vector<std::uint8_t>& data) {
    append(data.data(), data.size());
}

void Buffer::append(std::string_view text) {
    append(text.data(), text.size());
}

void Buffer::retrieve(std::size_t length) {
    if (length > readableBytes()) {
        throw std::out_of_range("buffer underflow");
    }

    read_index_ += length;
    compactIfNeeded();
}

void Buffer::retrieveAll() {
    buffer_.clear();
    read_index_ = 0;
}

auto Buffer::retrieveAsString(std::size_t length) -> std::string {
    if (length > readableBytes()) {
        throw std::out_of_range("buffer underflow");
    }

    const std::string result(
        reinterpret_cast<const char*>(peek()),
        reinterpret_cast<const char*>(peek() + length));
    retrieve(length);
    return result;
}

auto Buffer::retrieveAllAsString() -> std::string {
    return retrieveAsString(readableBytes());
}

auto Buffer::retrieveBytes(std::size_t length) -> std::vector<std::uint8_t> {
    if (length > readableBytes()) {
        throw std::out_of_range("buffer underflow");
    }

    std::vector<std::uint8_t> result(peek(), peek() + length);
    retrieve(length);
    return result;
}

auto Buffer::readFd(int fd) -> std::ptrdiff_t {
    std::uint8_t temp[4096];
    const auto bytes_read = ::recv(fd, temp, sizeof(temp), 0);
    if (bytes_read > 0) {
        append(temp, static_cast<std::size_t>(bytes_read));
    }
    return bytes_read;
}

auto Buffer::writeFd(int fd) -> std::ptrdiff_t {
    if (empty()) {
        return 0;
    }

    const auto bytes_written = ::send(
        fd,
        reinterpret_cast<const char*>(peek()),
        readableBytes(),
        MSG_NOSIGNAL);
    if (bytes_written > 0) {
        retrieve(static_cast<std::size_t>(bytes_written));
    }
    return bytes_written;
}

void Buffer::compactIfNeeded() {
    if (read_index_ == 0) {
        return;
    }

    if (read_index_ >= buffer_.size()) {
        retrieveAll();
        return;
    }

    if (read_index_ > buffer_.size() / 2) {
        std::move(buffer_.begin() + static_cast<std::ptrdiff_t>(read_index_),
                  buffer_.end(),
                  buffer_.begin());
        buffer_.resize(readableBytes());
        read_index_ = 0;
    }
}

}  // namespace media_relay
