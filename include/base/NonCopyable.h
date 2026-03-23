#pragma once

namespace media_relay {

class NonCopyable {
protected:
    constexpr NonCopyable() = default;
    ~NonCopyable() = default;

public:
    NonCopyable(const NonCopyable&) = delete;
    auto operator=(const NonCopyable&) -> NonCopyable& = delete;
    NonCopyable(NonCopyable&&) = delete;
    auto operator=(NonCopyable&&) -> NonCopyable& = delete;
};

}  // namespace media_relay
