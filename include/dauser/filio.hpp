#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <exception>

namespace filio {
    namespace extra {
        std::filesystem::path script_path();

        class FileError : public std::exception {
            std::string msg;
        public:
            explicit FileError(const std::string& message);
            const char* what() const noexcept override;
        };

        bool file_exists(const std::filesystem::path& filename);
        bool is_readable(const std::filesystem::path& filename);
        bool is_writable(const std::filesystem::path& filename);
        std::filesystem::path absolute_path(const std::string& path);
    }

    void write(const std::filesystem::path& filename, const std::string& text);
    std::string read(const std::filesystem::path& filename);
    void append(const std::filesystem::path& filename, const std::string& text);

    void bin_write(const std::filesystem::path& filename, const std::vector<char>& data);
    std::vector<char> bin_read(const std::filesystem::path& filename);
    void bin_append(const std::filesystem::path& filename, const std::vector<char>& data);
}
