#include "stream/StreamManager.h"

namespace media_relay {

auto StreamManager::registerPublisher(
    const std::string& stream_id,
    std::string publisher_id) -> bool {
    return getOrCreateSession(stream_id)->setPublisher(std::move(publisher_id));
}

void StreamManager::unregisterPublisher(const std::string& stream_id) {
    std::shared_ptr<StreamSession> session;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto iterator = sessions_.find(stream_id);
        if (iterator == sessions_.end()) {
            return;
        }
        session = iterator->second;
    }

    session->clearPublisher();
    removeIdleSession(stream_id);
}

void StreamManager::addSubscriber(
    const std::string& stream_id,
    const std::shared_ptr<Subscriber>& subscriber) {
    getOrCreateSession(stream_id)->addSubscriber(subscriber);
}

void StreamManager::removeSubscriber(
    const std::string& stream_id,
    const std::string& subscriber_id) {
    std::shared_ptr<StreamSession> session;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto iterator = sessions_.find(stream_id);
        if (iterator == sessions_.end()) {
            return;
        }
        session = iterator->second;
    }

    session->removeSubscriber(subscriber_id);
    removeIdleSession(stream_id);
}

auto StreamManager::publishFrame(const StreamFrame& frame) -> PublishResult {
    std::shared_ptr<StreamSession> session;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto iterator = sessions_.find(frame.stream_id);
        if (iterator == sessions_.end()) {
            return {};
        }
        session = iterator->second;
    }
    return session->publishFrame(frame);
}

auto StreamManager::sessionCount() const -> std::size_t {
    std::lock_guard<std::mutex> lock(mutex_);
    return sessions_.size();
}

auto StreamManager::subscriberCount(const std::string& stream_id) const -> std::size_t {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto iterator = sessions_.find(stream_id);
    return iterator != sessions_.end() ? iterator->second->subscriberCount() : 0;
}

auto StreamManager::getOrCreateSession(const std::string& stream_id)
    -> std::shared_ptr<StreamSession> {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto iterator = sessions_.find(stream_id);
    if (iterator != sessions_.end()) {
        return iterator->second;
    }

    auto session = std::make_shared<StreamSession>(stream_id);
    sessions_[stream_id] = session;
    return session;
}

void StreamManager::removeIdleSession(const std::string& stream_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto iterator = sessions_.find(stream_id);
    if (iterator != sessions_.end() && iterator->second->isIdle()) {
        sessions_.erase(iterator);
    }
}

}  // namespace media_relay
