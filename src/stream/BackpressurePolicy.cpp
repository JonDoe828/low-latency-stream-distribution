#include "stream/BackpressurePolicy.h"

namespace media_relay {

BackpressurePolicy::BackpressurePolicy(
    std::size_t max_queue_depth,
    bool disconnect_on_overflow)
    : max_queue_depth_(max_queue_depth),
      disconnect_on_overflow_(disconnect_on_overflow) {}

auto BackpressurePolicy::maxQueueDepth() const -> std::size_t {
    return max_queue_depth_;
}

auto BackpressurePolicy::pushFrame(
    std::deque<StreamFrame>& queue,
    const StreamFrame& frame) const -> OverflowAction {
    if (max_queue_depth_ == 0) {
        return OverflowAction::Disconnect;
    }

    if (queue.size() < max_queue_depth_) {
        queue.push_back(frame);
        return OverflowAction::Enqueued;
    }

    if (disconnect_on_overflow_) {
        return OverflowAction::Disconnect;
    }

    queue.pop_front();
    queue.push_back(frame);
    return OverflowAction::DroppedOldest;
}

}  // namespace media_relay
