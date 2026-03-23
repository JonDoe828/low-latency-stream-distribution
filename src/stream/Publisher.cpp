#include "stream/Publisher.h"

#include <utility>

namespace media_relay {

Publisher::Publisher(std::string id) : id_(std::move(id)) {}

auto Publisher::id() const -> const std::string& {
    return id_;
}

}  // namespace media_relay
