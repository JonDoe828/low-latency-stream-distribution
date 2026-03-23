#include "stream/StreamFrame.h"

namespace media_relay {

auto StreamFrame::sizeBytes() const -> std::size_t {
    return payload.size();
}

auto StreamFrame::toMessage() const -> Message {
    return Message::makeFrame(stream_id, sequence_number, payload, timestamp_us);
}

auto StreamFrame::fromMessage(const Message& message) -> StreamFrame {
    return StreamFrame {
        .stream_id = message.stream_id,
        .sequence_number = message.sequence_number,
        .timestamp_us = message.timestamp_us,
        .payload = message.payload,
    };
}

}  // namespace media_relay
