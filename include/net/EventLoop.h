#pragma once

#include <atomic>

namespace media_relay {

class EventLoop {
public:
    void loop();
    void stop();

    [[nodiscard]] auto running() const -> bool;

private:
    std::atomic<bool> running_ {false};
};

}  // namespace media_relay
