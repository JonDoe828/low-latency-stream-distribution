#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <vector>

#include "stream/StreamManager.h"

TEST_CASE("StreamManager fans out frames to all subscribers") {
    media_relay::StreamManager manager;
    std::vector<std::uint64_t> first_sequences;
    std::vector<std::uint64_t> second_sequences;

    auto first = std::make_shared<media_relay::Subscriber>(
        "sub-1",
        [&first_sequences](const media_relay::StreamFrame& frame) {
            first_sequences.push_back(frame.sequence_number);
            return true;
        });

    auto second = std::make_shared<media_relay::Subscriber>(
        "sub-2",
        [&second_sequences](const media_relay::StreamFrame& frame) {
            second_sequences.push_back(frame.sequence_number);
            return true;
        });

    REQUIRE(manager.registerPublisher("demo", "pub-1"));
    manager.addSubscriber("demo", first);
    manager.addSubscriber("demo", second);

    const auto result = manager.publishFrame(media_relay::StreamFrame {
        .stream_id = "demo",
        .sequence_number = 7,
        .timestamp_us = 99,
        .payload = std::vector<std::uint8_t> {'x', 'y'},
    });

    REQUIRE(result.delivered == 2);
    REQUIRE(result.dropped == 0);
    REQUIRE(first_sequences == std::vector<std::uint64_t> {7});
    REQUIRE(second_sequences == std::vector<std::uint64_t> {7});
    REQUIRE(manager.subscriberCount("demo") == 2);
}

TEST_CASE("StreamManager rejects a second publisher on the same stream") {
    media_relay::StreamManager manager;

    REQUIRE(manager.registerPublisher("demo", "pub-1"));
    REQUIRE_FALSE(manager.registerPublisher("demo", "pub-2"));
}
