#pragma once

#include <vector>

#include "net/Channel.h"

namespace media_relay {

class Poller {
public:
    void addChannel(const Channel& channel);
    [[nodiscard]] auto channels() const -> const std::vector<Channel>&;

private:
    std::vector<Channel> channels_;
};

}  // namespace media_relay
