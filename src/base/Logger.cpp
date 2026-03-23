#include "base/Logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>

namespace media_relay {
namespace {

auto loggerMutex() -> std::mutex& {
    static std::mutex mutex;
    return mutex;
}

auto levelToString(LogLevel level) -> const char* {
    switch (level) {
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warn:
            return "WARN";
        case LogLevel::Error:
            return "ERROR";
    }
    return "INFO";
}

}  // namespace

void Logger::log(LogLevel level, std::string_view message) {
    const auto now = std::chrono::system_clock::now();
    const std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    std::tm local_time {};
    localtime_r(&now_time, &local_time);

    std::lock_guard<std::mutex> lock(loggerMutex());
    std::clog << std::put_time(&local_time, "%F %T")
              << " [" << levelToString(level) << "] "
              << message << '\n';
}

void Logger::debug(std::string_view message) {
    log(LogLevel::Debug, message);
}

void Logger::info(std::string_view message) {
    log(LogLevel::Info, message);
}

void Logger::warn(std::string_view message) {
    log(LogLevel::Warn, message);
}

void Logger::error(std::string_view message) {
    log(LogLevel::Error, message);
}

}  // namespace media_relay
