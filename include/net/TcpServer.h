#pragma once

#include <cstdint>
#include <string>

#include "net/Acceptor.h"

namespace media_relay {

class TcpServer {
public:
    TcpServer(std::string host, std::uint16_t port, int backlog);

    auto start() -> bool;
    void stop();

private:
    Acceptor acceptor_;
};

}  // namespace media_relay
