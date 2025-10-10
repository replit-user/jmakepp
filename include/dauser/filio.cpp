#include "filio.hpp"
#include <fstream>

namespace filio {
namespace extra {

    FileError::FileError(const std::string& message) : msg(message) {}
    const char* FileError::what() const noexcept { return msg.c_str(); }

    std::filesystem::path script_path() {
        return std::filesystem::current_path();
    }

    bool file_exists(const std::filesystem::path& filename) {
        return std::filesystem::exists(filename);
    }

    bool is_readable(const std::filesystem::path& filename) {
        std::error_code ec;
        auto perms = std::filesystem::status(filename, ec).permissions();
        if (ec) return false;
        return (perms & std::filesystem::perms::owner_read) != std::filesystem::perms::none;
    }

    bool is_writable(const std::filesystem::path& filename) {
        std::error_code ec;
        auto perms = std::filesystem::status(filename, ec).permissions();
        if (ec) return false;
        return (perms & std::filesystem::perms::owner_write) != std::filesystem::perms::none;
    }

    std::filesystem::path absolute_path(const std::string& path) {
        std::error_code ec;
        auto abs = std::filesystem::absolute(std::filesystem::path(path), ec);
        if (ec)
            throw FileError("Failed to resolve absolute path: " + path);
        return abs;
    }

} // namespace extra

void write(const std::filesystem::path& filename, const std::string& text) {
    std::ofstream stream(filename, std::ios::binary);
    if (!stream)
        throw extra::FileError("Failed to open file for writing: " + filename.string());
    stream.write(text.data(), static_cast<std::streamsize>(text.size()));
}

std::string read(const std::filesystem::path& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in)
        throw extra::FileError("Failed to open file for reading: " + filename.string());
    return std::string(std::istreambuf_iterator<char>(in), {});
}

void append(const std::filesystem::path& filename, const std::string& text) {
    std::ofstream stream(filename, std::ios::binary | std::ios::app);
    if (!stream)
        throw extra::FileError("Failed to open file for appending: " + filename.string());
    stream.write(text.data(), static_cast<std::streamsize>(text.size()));
}

void bin_write(const std::filesystem::path& filename, const std::vector<char>& data) {
    std::ofstream stream(filename, std::ios::binary);
    if (!stream)
        throw extra::FileError("Failed to open file for binary writing: " + filename.string());
    stream.write(data.data(), static_cast<std::streamsize>(data.size()));
}

std::vector<char> bin_read(const std::filesystem::path& filename) {
    std::ifstream stream(filename, std::ios::binary);
    if (!stream)
        throw extra::FileError("Failed to open file for binary reading: " + filename.string());
    return {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
}

void bin_append(const std::filesystem::path& filename, const std::vector<char>& data) {
    std::ofstream stream(filename, std::ios::binary | std::ios::app);
    if (!stream)
        throw extra::FileError("Failed to open file for binary appending: " + filename.string());
    stream.write(data.data(), static_cast<std::streamsize>(data.size()));
}

} // namespace filio