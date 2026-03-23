#include "net/Acceptor.h"

namespace media_relay {

Acceptor::Acceptor(std::string host, std::uint16_t port, int backlog)
    : host_(std::move(host)),
      port_(port),
      backlog_(backlog) {}

auto Acceptor::start() -> bool {
    socket_ = Socket::createTcp();
    if (!socket_.valid()) {
        return false;
    }
    socket_.setReuseAddr(true);
    return socket_.bind(host_, port_) && socket_.listen(backlog_);
}

auto Acceptor::accept() -> Socket {
    return socket_.accept();
}

void Acceptor::close() {
    socket_.close();
}

auto Acceptor::fd() const -> int {
    return socket_.fd();
}

}  // namespace media_relay
