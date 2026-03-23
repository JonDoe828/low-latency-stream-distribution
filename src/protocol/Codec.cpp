#include "protocol/Codec.h"

#include <array>
#include <cstring>

namespace media_relay {
namespace {

void appendUint8(std::vector<std::uint8_t>& output, std::uint8_t value) {
    output.push_back(value);
}

void appendUint16(std::vector<std::uint8_t>& output, std::uint16_t value) {
    output.push_back(static_cast<std::uint8_t>((value >> 8U) & 0xffU));
    output.push_back(static_cast<std::uint8_t>(value & 0xffU));
}

void appendUint32(std::vector<std::uint8_t>& output, std::uint32_t value) {
    output.push_back(static_cast<std::uint8_t>((value >> 24U) & 0xffU));
    output.push_back(static_cast<std::uint8_t>((value >> 16U) & 0xffU));
    output.push_back(static_cast<std::uint8_t>((value >> 8U) & 0xffU));
    output.push_back(static_cast<std::uint8_t>(value & 0xffU));
}

void appendUint64(std::vector<std::uint8_t>& output, std::uint64_t value) {
    for (int shift = 56; shift >= 0; shift -= 8) {
        output.push_back(static_cast<std::uint8_t>((value >> shift) & 0xffU));
    }
}

auto readUint16(const std::uint8_t* input) -> std::uint16_t {
    return static_cast<std::uint16_t>(input[0]) << 8U
         | static_cast<std::uint16_t>(input[1]);
}

auto readUint32(const std::uint8_t* input) -> std::uint32_t {
    return static_cast<std::uint32_t>(input[0]) << 24U
         | static_cast<std::uint32_t>(input[1]) << 16U
         | static_cast<std::uint32_t>(input[2]) << 8U
         | static_cast<std::uint32_t>(input[3]);
}

auto readUint64(const std::uint8_t* input) -> std::uint64_t {
    std::uint64_t value = 0;
    for (int index = 0; index < 8; ++index) {
        value = (value << 8U) | static_cast<std::uint64_t>(input[index]);
    }
    return value;
}

}  // namespace

auto Codec::encode(const Message& message) -> std::vector<std::uint8_t> {
    const auto stream_id_size = message.stream_id.size();
    const auto text_size = message.text.size();
    const auto payload_size = message.payload.size();

    std::vector<std::uint8_t> output;
    output.reserve(kHeaderSize + stream_id_size + text_size + payload_size);

    appendUint32(output, kMagic);
    appendUint8(output, kVersion);
    appendUint8(output, static_cast<std::uint8_t>(message.type));
    appendUint16(output, 0);
    appendUint16(output, static_cast<std::uint16_t>(stream_id_size));
    appendUint16(output, static_cast<std::uint16_t>(text_size));
    appendUint32(output, static_cast<std::uint32_t>(payload_size));
    appendUint64(output, message.sequence_number);
    appendUint64(output, message.timestamp_us);

    output.insert(output.end(), message.stream_id.begin(), message.stream_id.end());
    output.insert(output.end(), message.text.begin(), message.text.end());
    output.insert(output.end(), message.payload.begin(), message.payload.end());
    return output;
}

void Codec::encodeToBuffer(const Message& message, Buffer& buffer) {
    const auto encoded = encode(message);
    buffer.append(encoded);
}

auto Codec::tryDecode(
    Buffer& buffer,
    Message& message,
    std::string* error_message) -> DecodeStatus {
    if (buffer.readableBytes() < kHeaderSize) {
        return DecodeStatus::NeedMoreData;
    }

    const std::uint8_t* header = buffer.peek();
    const auto magic = readUint32(header);
    if (magic != kMagic) {
        if (error_message != nullptr) {
            *error_message = "invalid magic";
        }
        return DecodeStatus::Error;
    }

    const auto version = header[4];
    if (version != kVersion) {
        if (error_message != nullptr) {
            *error_message = "unsupported version";
        }
        return DecodeStatus::Error;
    }

    const auto type = static_cast<CommandType>(header[5]);
    const auto stream_id_length = static_cast<std::size_t>(readUint16(header + 8));
    const auto text_length = static_cast<std::size_t>(readUint16(header + 10));
    const auto payload_length = static_cast<std::size_t>(readUint32(header + 12));
    const auto sequence_number = readUint64(header + 16);
    const auto timestamp_us = readUint64(header + 24);

    if (stream_id_length > kMaxFieldSize || text_length > kMaxFieldSize) {
        if (error_message != nullptr) {
            *error_message = "field too large";
        }
        return DecodeStatus::Error;
    }

    if (payload_length > kMaxPayloadSize) {
        if (error_message != nullptr) {
            *error_message = "payload too large";
        }
        return DecodeStatus::Error;
    }

    const auto total_size = kHeaderSize + stream_id_length + text_length + payload_length;
    if (buffer.readableBytes() < total_size) {
        return DecodeStatus::NeedMoreData;
    }

    message = Message {};
    message.type = type;
    message.sequence_number = sequence_number;
    message.timestamp_us = timestamp_us;

    buffer.retrieve(kHeaderSize);
    message.stream_id = buffer.retrieveAsString(stream_id_length);
    message.text = buffer.retrieveAsString(text_length);
    message.payload = buffer.retrieveBytes(payload_length);
    return DecodeStatus::Ok;
}

}  // namespace media_relay
