#include "net/Channel.h"

namespace media_relay {

Channel::Channel(int fd) : fd_(fd) {}

auto Channel::fd() const -> int {
    return fd_;
}

}  // namespace media_relay
