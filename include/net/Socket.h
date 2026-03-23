#pragma once

#include <cstdint>
#include <string>

#include "base/NonCopyable.h"

namespace media_relay {

class Socket : public NonCopyable {
public:
    Socket() = default;
    explicit Socket(int fd);
    Socket(Socket&& other) noexcept;
    auto operator=(Socket&& other) noexcept -> Socket&;
    ~Socket();

    [[nodiscard]] auto fd() const -> int;
    [[nodiscard]] auto valid() const -> bool;

    static auto createTcp() -> Socket;

    void setReuseAddr(bool enabled) const;
    auto bind(const std::string& host, std::uint16_t port) const -> bool;
    auto listen(int backlog) const -> bool;
    auto accept() const -> Socket;
    auto connect(const std::string& host, std::uint16_t port) const -> bool;

    void close();

private:
    int fd_ {-1};
};

}  // namespace media_relay
