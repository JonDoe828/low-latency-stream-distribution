#include "protocol/Message.h"

namespace media_relay {

auto Message::makePublish(std::string stream_id, std::string metadata) -> Message {
    Message message;
    message.type = CommandType::Publish;
    message.stream_id = std::move(stream_id);
    message.text = std::move(metadata);
    return message;
}

auto Message::makeSubscribe(std::string stream_id) -> Message {
    Message message;
    message.type = CommandType::Subscribe;
    message.stream_id = std::move(stream_id);
    return message;
}

auto Message::makeUnsubscribe(std::string stream_id) -> Message {
    Message message;
    message.type = CommandType::Unsubscribe;
    message.stream_id = std::move(stream_id);
    return message;
}

auto Message::makeFrame(
    std::string stream_id,
    std::uint64_t sequence_number,
    std::vector<std::uint8_t> payload,
    std::uint64_t timestamp_us) -> Message {
    Message message;
    message.type = CommandType::Frame;
    message.stream_id = std::move(stream_id);
    message.sequence_number = sequence_number;
    message.timestamp_us = timestamp_us;
    message.payload = std::move(payload);
    return message;
}

auto Message::makeHeartbeat() -> Message {
    Message message;
    message.type = CommandType::Heartbeat;
    return message;
}

auto Message::makeAck(std::string text) -> Message {
    Message message;
    message.type = CommandType::Ack;
    message.text = std::move(text);
    return message;
}

auto Message::makeError(std::string text, std::string stream_id) -> Message {
    Message message;
    message.type = CommandType::Error;
    message.text = std::move(text);
    message.stream_id = std::move(stream_id);
    return message;
}

}  // namespace media_relay
