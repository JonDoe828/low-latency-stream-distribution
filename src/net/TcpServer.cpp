#include "net/TcpServer.h"

namespace media_relay {

TcpServer::TcpServer(std::string host, std::uint16_t port, int backlog)
    : acceptor_(std::move(host), port, backlog) {}

auto TcpServer::start() -> bool {
    return acceptor_.start();
}

void TcpServer::stop() {
    acceptor_.close();
}

}  // namespace media_relay
