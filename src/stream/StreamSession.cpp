#include "stream/StreamSession.h"

#include <vector>

namespace media_relay {

StreamSession::StreamSession(std::string stream_id)
    : stream_id_(std::move(stream_id)) {}

auto StreamSession::streamId() const -> const std::string& {
    return stream_id_;
}

auto StreamSession::setPublisher(std::string publisher_id) -> bool {
    std::lock_guard<std::mutex> lock(mutex_);
    if (publisher_id_.has_value() && publisher_id_ != publisher_id) {
        return false;
    }
    publisher_id_ = std::move(publisher_id);
    return true;
}

void StreamSession::clearPublisher() {
    std::lock_guard<std::mutex> lock(mutex_);
    publisher_id_.reset();
}

void StreamSession::addSubscriber(const std::shared_ptr<Subscriber>& subscriber) {
    std::lock_guard<std::mutex> lock(mutex_);
    subscribers_[subscriber->id()] = subscriber;
}

void StreamSession::removeSubscriber(const std::string& subscriber_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    subscribers_.erase(subscriber_id);
}

auto StreamSession::publishFrame(const StreamFrame& frame) -> PublishResult {
    std::vector<std::shared_ptr<Subscriber>> subscribers;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        subscribers.reserve(subscribers_.size());
        for (const auto& [id, subscriber] : subscribers_) {
            static_cast<void>(id);
            subscribers.push_back(subscriber);
        }
    }

    PublishResult result;
    for (const auto& subscriber : subscribers) {
        if (subscriber->deliver(frame)) {
            ++result.delivered;
        } else {
            ++result.dropped;
        }
    }
    return result;
}

auto StreamSession::subscriberCount() const -> std::size_t {
    std::lock_guard<std::mutex> lock(mutex_);
    return subscribers_.size();
}

auto StreamSession::hasPublisher() const -> bool {
    std::lock_guard<std::mutex> lock(mutex_);
    return publisher_id_.has_value();
}

auto StreamSession::isIdle() const -> bool {
    std::lock_guard<std::mutex> lock(mutex_);
    return !publisher_id_.has_value() && subscribers_.empty();
}

}  // namespace media_relay
