#pragma once

#include <functional>
#include <string>

#include "stream/StreamFrame.h"

namespace media_relay {

class Subscriber {
public:
    using DeliveryCallback = std::function<bool(const StreamFrame&)>;

    Subscriber(std::string id, DeliveryCallback callback);

    [[nodiscard]] auto id() const -> const std::string&;
    [[nodiscard]] auto deliver(const StreamFrame& frame) const -> bool;

private:
    std::string id_;
    DeliveryCallback callback_;
};

}  // namespace media_relay
