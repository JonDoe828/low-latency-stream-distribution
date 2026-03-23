#pragma once

#include <cstddef>
#include <deque>

#include "stream/StreamFrame.h"

namespace media_relay {

enum class OverflowAction {
    Enqueued,
    DroppedOldest,
    DroppedNewest,
    Disconnect,
};

class BackpressurePolicy {
public:
    explicit BackpressurePolicy(
        std::size_t max_queue_depth = 64,
        bool disconnect_on_overflow = false);

    [[nodiscard]] auto maxQueueDepth() const -> std::size_t;
    [[nodiscard]] auto pushFrame(
        std::deque<StreamFrame>& queue,
        const StreamFrame& frame) const -> OverflowAction;

private:
    std::size_t max_queue_depth_;
    bool disconnect_on_overflow_;
};

}  // namespace media_relay
