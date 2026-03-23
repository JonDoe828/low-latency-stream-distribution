#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "protocol/Command.h"

namespace media_relay {

struct Message {
    CommandType type {CommandType::Heartbeat};
    std::string stream_id;
    std::uint64_t sequence_number {0};
    std::uint64_t timestamp_us {0};
    std::string text;
    std::vector<std::uint8_t> payload;

    static auto makePublish(std::string stream_id, std::string metadata = {}) -> Message;
    static auto makeSubscribe(std::string stream_id) -> Message;
    static auto makeUnsubscribe(std::string stream_id) -> Message;
    static auto makeFrame(
        std::string stream_id,
        std::uint64_t sequence_number,
        std::vector<std::uint8_t> payload,
        std::uint64_t timestamp_us) -> Message;
    static auto makeHeartbeat() -> Message;
    static auto makeAck(std::string text = {}) -> Message;
    static auto makeError(std::string text, std::string stream_id = {}) -> Message;
};

}  // namespace media_relay
