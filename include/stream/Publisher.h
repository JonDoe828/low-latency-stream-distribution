#pragma once

#include <string>

namespace media_relay {

class Publisher {
public:
    explicit Publisher(std::string id = {});

    [[nodiscard]] auto id() const -> const std::string&;

private:
    std::string id_;
};

}  // namespace media_relay
