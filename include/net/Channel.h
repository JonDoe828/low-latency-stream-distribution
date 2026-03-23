#pragma once

namespace media_relay {

class Channel {
public:
    explicit Channel(int fd = -1);

    [[nodiscard]] auto fd() const -> int;

private:
    int fd_;
};

}  // namespace media_relay
