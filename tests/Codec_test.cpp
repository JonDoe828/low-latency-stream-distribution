#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

#include "net/Buffer.h"
#include "protocol/Codec.h"

TEST_CASE("Codec round trips a media frame message") {
    const media_relay::Message input = media_relay::Message::makeFrame(
        "live/stream",
        42,
        std::vector<std::uint8_t> {'a', 'b', 'c'},
        123456);

    media_relay::Buffer buffer;
    media_relay::Codec::encodeToBuffer(input, buffer);

    media_relay::Message output;
    REQUIRE(
        media_relay::Codec::tryDecode(buffer, output) ==
        media_relay::Codec::DecodeStatus::Ok);
    REQUIRE(output.type == media_relay::CommandType::Frame);
    REQUIRE(output.stream_id == "live/stream");
    REQUIRE(output.sequence_number == 42);
    REQUIRE(output.timestamp_us == 123456);
    REQUIRE(output.payload == std::vector<std::uint8_t> {'a', 'b', 'c'});
}

TEST_CASE("Codec waits for a complete frame before decoding") {
    const auto encoded = media_relay::Codec::encode(
        media_relay::Message::makeSubscribe("demo"));

    media_relay::Buffer buffer;
    buffer.append(encoded.data(), 5);

    media_relay::Message output;
    REQUIRE(
        media_relay::Codec::tryDecode(buffer, output) ==
        media_relay::Codec::DecodeStatus::NeedMoreData);

    buffer.append(encoded.data() + 5, encoded.size() - 5);
    REQUIRE(
        media_relay::Codec::tryDecode(buffer, output) ==
        media_relay::Codec::DecodeStatus::Ok);
    REQUIRE(output.type == media_relay::CommandType::Subscribe);
    REQUIRE(output.stream_id == "demo");
}
