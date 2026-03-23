#pragma once

#include <atomic>
#include <cstdint>
#include <thread>

#include "app/Config.h"
#include "base/NonCopyable.h"
#include "net/Socket.h"
#include "stream/BackpressurePolicy.h"
#include "stream/StreamManager.h"

namespace media_relay {

class StreamRelayServer : public NonCopyable {
public:
    explicit StreamRelayServer(Config config);
    ~StreamRelayServer();

    auto start() -> bool;
    void stop();
    void wait();

private:
    void acceptLoop();
    void handleClient(int client_fd, std::string connection_id);

    Config config_;
    std::atomic<bool> running_ {false};
    std::thread accept_thread_;
    Socket listen_socket_;
    StreamManager stream_manager_;
    BackpressurePolicy backpressure_policy_;
    std::atomic<std::uint64_t> next_connection_id_ {1};
};

}  // namespace media_relay
