#include "net/EventLoop.h"

#include <chrono>
#include <thread>

namespace media_relay {

void EventLoop::loop() {
    running_.store(true);
    while (running_.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void EventLoop::stop() {
    running_.store(false);
}

auto EventLoop::running() const -> bool {
    return running_.load();
}

}  // namespace media_relay
