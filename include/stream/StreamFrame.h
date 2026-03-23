#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "protocol/Message.h"

namespace media_relay {

struct StreamFrame {
    std::string stream_id;
    std::uint64_t sequence_number {0};
    std::uint64_t timestamp_us {0};
    std::vector<std::uint8_t> payload;

    [[nodiscard]] auto sizeBytes() const -> std::size_t;
    [[nodiscard]] auto toMessage() const -> Message;
    static auto fromMessage(const Message& message) -> StreamFrame;
};

}  // namespace media_relay
