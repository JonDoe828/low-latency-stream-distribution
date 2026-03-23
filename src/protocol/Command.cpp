#include "protocol/Command.h"

namespace media_relay {

auto toString(CommandType command) -> std::string_view {
    switch (command) {
        case CommandType::Publish:
            return "PUBLISH";
        case CommandType::Subscribe:
            return "SUBSCRIBE";
        case CommandType::Unsubscribe:
            return "UNSUBSCRIBE";
        case CommandType::Frame:
            return "FRAME";
        case CommandType::Heartbeat:
            return "HEARTBEAT";
        case CommandType::Error:
            return "ERROR";
        case CommandType::Ack:
            return "ACK";
    }
    return "UNKNOWN";
}

}  // namespace media_relay
