#include "stream/Subscriber.h"

namespace media_relay {

Subscriber::Subscriber(std::string id, DeliveryCallback callback)
    : id_(std::move(id)), callback_(std::move(callback)) {}

auto Subscriber::id() const -> const std::string& {
    return id_;
}

auto Subscriber::deliver(const StreamFrame& frame) const -> bool {
    return callback_ != nullptr ? callback_(frame) : false;
}

}  // namespace media_relay
