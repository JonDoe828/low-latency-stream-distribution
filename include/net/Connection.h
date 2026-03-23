#pragma once

#include <cstdint>
#include <vector>

#include "net/Buffer.h"
#include "net/Socket.h"

namespace media_relay {

class Connection {
public:
    explicit Connection(Socket socket);

    [[nodiscard]] auto fd() const -> int;
    [[nodiscard]] auto inputBuffer() -> Buffer&;
    [[nodiscard]] auto outputBuffer() -> Buffer&;

    auto send(const std::vector<std::uint8_t>& bytes) -> bool;
    void close();

private:
    Socket socket_;
    Buffer input_buffer_;
    Buffer output_buffer_;
};

}  // namespace media_relay
