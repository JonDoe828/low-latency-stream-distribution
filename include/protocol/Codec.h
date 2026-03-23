#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "net/Buffer.h"
#include "protocol/Message.h"

namespace media_relay {

class Codec {
public:
    static constexpr std::uint32_t kMagic = 0x4d524c59;  // MRLY
    static constexpr std::uint8_t kVersion = 1;
    static constexpr std::size_t kHeaderSize = 32;
    static constexpr std::size_t kMaxFieldSize = 64 * 1024;
    static constexpr std::size_t kMaxPayloadSize = 2 * 1024 * 1024;

    enum class DecodeStatus {
        NeedMoreData,
        Ok,
        Error,
    };

    [[nodiscard]] static auto encode(const Message& message) -> std::vector<std::uint8_t>;
    static void encodeToBuffer(const Message& message, Buffer& buffer);
    [[nodiscard]] static auto tryDecode(
        Buffer& buffer,
        Message& message,
        std::string* error_message = nullptr) -> DecodeStatus;
};

}  // namespace media_relay
