#include <catch2/catch_test_macros.hpp>

#include <string>

#include "net/Buffer.h"

TEST_CASE("Buffer appends and retrieves data in order") {
    media_relay::Buffer buffer;
    buffer.append("hello");
    buffer.append(", world");

    REQUIRE(buffer.readableBytes() == 12);
    REQUIRE(buffer.retrieveAsString(5) == "hello");
    REQUIRE(buffer.retrieveAllAsString() == ", world");
    REQUIRE(buffer.empty());
}
