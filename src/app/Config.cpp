#include "app/Config.h"

#include <cstdlib>
#include <stdexcept>
#include <string>

namespace media_relay {

auto Config::fromArgv(int argc, char** argv) -> Config {
    Config config;

    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];
        if (argument == "--host" && index + 1 < argc) {
            config.host = argv[++index];
        } else if (argument == "--port" && index + 1 < argc) {
            config.port = static_cast<std::uint16_t>(std::stoi(argv[++index]));
        } else if (argument == "--backlog" && index + 1 < argc) {
            config.backlog = std::stoi(argv[++index]);
        } else if (argument == "--queue" && index + 1 < argc) {
            config.max_subscriber_queue =
                static_cast<std::size_t>(std::stoul(argv[++index]));
        } else if (argument == "--disconnect-slow-subscriber") {
            config.disconnect_slow_subscriber = true;
        } else {
            throw std::invalid_argument("unknown argument: " + argument);
        }
    }

    return config;
}

}  // namespace media_relay
