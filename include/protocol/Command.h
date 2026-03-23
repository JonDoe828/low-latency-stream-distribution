#pragma once

#include <cstdint>
#include <string_view>

namespace media_relay {

enum class CommandType : std::uint8_t {
    Publish = 1,
    Subscribe = 2,
    Unsubscribe = 3,
    Frame = 4,
    Heartbeat = 5,
    Error = 6,
    Ack = 7,
};

[[nodiscard]] auto toString(CommandType command) -> std::string_view;

}  // namespace media_relay
