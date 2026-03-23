#include <catch2/catch_test_macros.hpp>

#include <deque>
#include <string>
#include <vector>

#include "stream/BackpressurePolicy.h"

namespace {

auto makeFrame(std::uint64_t sequence_number) -> media_relay::StreamFrame {
    return media_relay::StreamFrame {
        .stream_id = "demo",
        .sequence_number = sequence_number,
        .timestamp_us = sequence_number,
        .payload = std::vector<std::uint8_t> {'x'},
    };
}

}  // namespace

TEST_CASE("BackpressurePolicy drops oldest frame when queue is full") {
    media_relay::BackpressurePolicy policy(2, false);
    std::deque<media_relay::StreamFrame> queue;

    REQUIRE(policy.pushFrame(queue, makeFrame(1)) == media_relay::OverflowAction::Enqueued);
    REQUIRE(policy.pushFrame(queue, makeFrame(2)) == media_relay::OverflowAction::Enqueued);
    REQUIRE(
        policy.pushFrame(queue, makeFrame(3)) == media_relay::OverflowAction::DroppedOldest);

    REQUIRE(queue.size() == 2);
    REQUIRE(queue.front().sequence_number == 2);
    REQUIRE(queue.back().sequence_number == 3);
}
