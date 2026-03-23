#include "base/Timestamp.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace media_relay {

Timestamp::Timestamp(std::int64_t microseconds) : microseconds_(microseconds) {}

auto Timestamp::now() -> Timestamp {
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    return Timestamp(
        std::chrono::duration_cast<std::chrono::microseconds>(now).count());
}

auto Timestamp::microseconds() const -> std::int64_t {
    return microseconds_;
}

auto Timestamp::toString() const -> std::string {
    const auto seconds = std::chrono::seconds(microseconds_ / 1000000);
    const auto micros = microseconds_ % 1000000;
    const std::time_t raw_time = seconds.count();

    std::tm local_time {};
    localtime_r(&raw_time, &local_time);

    std::ostringstream output;
    output << std::put_time(&local_time, "%F %T")
           << '.' << std::setw(6) << std::setfill('0') << micros;
    return output.str();
}

}  // namespace media_relay
