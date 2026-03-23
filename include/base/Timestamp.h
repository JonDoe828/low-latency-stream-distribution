#pragma once

#include <cstdint>
#include <string>

namespace media_relay {

class Timestamp {
public:
    explicit Timestamp(std::int64_t microseconds = 0);

    static auto now() -> Timestamp;

    [[nodiscard]] auto microseconds() const -> std::int64_t;
    [[nodiscard]] auto toString() const -> std::string;

private:
    std::int64_t microseconds_;
};

}  // namespace media_relay
