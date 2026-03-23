#pragma once

#include <cstddef>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

#include "stream/Subscriber.h"

namespace media_relay {

struct PublishResult {
    std::size_t delivered {0};
    std::size_t dropped {0};
};

class StreamSession {
public:
    explicit StreamSession(std::string stream_id);

    [[nodiscard]] auto streamId() const -> const std::string&;

    auto setPublisher(std::string publisher_id) -> bool;
    void clearPublisher();

    void addSubscriber(const std::shared_ptr<Subscriber>& subscriber);
    void removeSubscriber(const std::string& subscriber_id);

    [[nodiscard]] auto publishFrame(const StreamFrame& frame) -> PublishResult;
    [[nodiscard]] auto subscriberCount() const -> std::size_t;
    [[nodiscard]] auto hasPublisher() const -> bool;
    [[nodiscard]] auto isIdle() const -> bool;

private:
    std::string stream_id_;
    mutable std::mutex mutex_;
    std::optional<std::string> publisher_id_;
    std::unordered_map<std::string, std::shared_ptr<Subscriber>> subscribers_;
};

}  // namespace media_relay
