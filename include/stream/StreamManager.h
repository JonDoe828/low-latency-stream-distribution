#pragma once

#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "stream/StreamSession.h"

namespace media_relay {

class StreamManager {
public:
    auto registerPublisher(const std::string& stream_id, std::string publisher_id) -> bool;
    void unregisterPublisher(const std::string& stream_id);

    void addSubscriber(
        const std::string& stream_id,
        const std::shared_ptr<Subscriber>& subscriber);
    void removeSubscriber(const std::string& stream_id, const std::string& subscriber_id);

    [[nodiscard]] auto publishFrame(const StreamFrame& frame) -> PublishResult;
    [[nodiscard]] auto sessionCount() const -> std::size_t;
    [[nodiscard]] auto subscriberCount(const std::string& stream_id) const -> std::size_t;

private:
    auto getOrCreateSession(const std::string& stream_id) -> std::shared_ptr<StreamSession>;
    void removeIdleSession(const std::string& stream_id);

    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<StreamSession>> sessions_;
};

}  // namespace media_relay
