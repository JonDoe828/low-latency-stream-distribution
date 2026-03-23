#include "net/Poller.h"

namespace media_relay {

void Poller::addChannel(const Channel& channel) {
    channels_.push_back(channel);
}

auto Poller::channels() const -> const std::vector<Channel>& {
    return channels_;
}

}  // namespace media_relay
