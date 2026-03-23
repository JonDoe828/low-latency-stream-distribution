#include "net/Socket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <utility>

namespace media_relay {
namespace {

auto makeSockaddr(const std::string& host, std::uint16_t port) -> sockaddr_in {
    sockaddr_in address {};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if (host.empty() || host == "0.0.0.0") {
        address.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        ::inet_pton(AF_INET, host.c_str(), &address.sin_addr);
    }
    return address;
}

}  // namespace

Socket::Socket(int fd) : fd_(fd) {}

Socket::Socket(Socket&& other) noexcept : fd_(other.fd_) {
    other.fd_ = -1;
}

auto Socket::operator=(Socket&& other) noexcept -> Socket& {
    if (this != &other) {
        close();
        fd_ = other.fd_;
        other.fd_ = -1;
    }
    return *this;
}

Socket::~Socket() {
    close();
}

auto Socket::fd() const -> int {
    return fd_;
}

auto Socket::valid() const -> bool {
    return fd_ >= 0;
}

auto Socket::createTcp() -> Socket {
    return Socket(::socket(AF_INET, SOCK_STREAM, 0));
}

void Socket::setReuseAddr(bool enabled) const {
    const int value = enabled ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
}

auto Socket::bind(const std::string& host, std::uint16_t port) const -> bool {
    const auto address = makeSockaddr(host, port);
    return ::bind(fd_, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) == 0;
}

auto Socket::listen(int backlog) const -> bool {
    return ::listen(fd_, backlog) == 0;
}

auto Socket::accept() const -> Socket {
    return Socket(::accept(fd_, nullptr, nullptr));
}

auto Socket::connect(const std::string& host, std::uint16_t port) const -> bool {
    const auto address = makeSockaddr(host, port);
    return ::connect(fd_, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) == 0;
}

void Socket::close() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

}  // namespace media_relay
