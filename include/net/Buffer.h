#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace media_relay {

class Buffer {
public:
    Buffer() = default;

    [[nodiscard]] auto readableBytes() const -> std::size_t;
    [[nodiscard]] auto empty() const -> bool;
    [[nodiscard]] auto peek() const -> const std::uint8_t*;

    void append(const void* data, std::size_t length);
    void append(const std::vector<std::uint8_t>& data);
    void append(std::string_view text);

    void retrieve(std::size_t length);
    void retrieveAll();

    [[nodiscard]] auto retrieveAsString(std::size_t length) -> std::string;
    [[nodiscard]] auto retrieveAllAsString() -> std::string;
    [[nodiscard]] auto retrieveBytes(std::size_t length) -> std::vector<std::uint8_t>;

    auto readFd(int fd) -> std::ptrdiff_t;
    auto writeFd(int fd) -> std::ptrdiff_t;

private:
    void compactIfNeeded();

    std::vector<std::uint8_t> buffer_;
    std::size_t read_index_ {0};
};

}  // namespace media_relay
