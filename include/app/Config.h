#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace media_relay {

struct Config {
    std::string host {"0.0.0.0"};
    std::uint16_t port {9000};
    int backlog {128};
    std::size_t max_subscriber_queue {64};
    bool disconnect_slow_subscriber {false};

    static auto fromArgv(int argc, char** argv) -> Config;
};

}  // namespace media_relay
