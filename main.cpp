#include <atomic>
#include <chrono>
#include <csignal>
#include <exception>
#include <iostream>
#include <thread>

#include "app/Config.h"
#include "app/StreamRelayServer.h"

namespace {

std::atomic<bool> g_stop_requested {false};

void handleSignal(int) {
    g_stop_requested.store(true);
}

}  // namespace

int main(int argc, char** argv) {
    try {
        auto config = media_relay::Config::fromArgv(argc, argv);
        media_relay::StreamRelayServer server(config);
        if (!server.start()) {
            std::cerr << "failed to start stream relay server\n";
            return 1;
        }

        std::signal(SIGINT, handleSignal);
        std::signal(SIGTERM, handleSignal);

        while (!g_stop_requested.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        server.stop();
        server.wait();
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
