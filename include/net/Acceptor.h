#pragma once

#include <cstdint>
#include <string>

#include "net/Socket.h"

namespace media_relay {

class Acceptor {
public:
    Acceptor(std::string host, std::uint16_t port, int backlog);

    auto start() -> bool;
    auto accept() -> Socket;
    void close();

    [[nodiscard]] auto fd() const -> int;

private:
    std::string host_;
    std::uint16_t port_;
    int backlog_;
    Socket socket_;
};

}  // namespace media_relay
