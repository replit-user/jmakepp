#include "../include/dauser/config.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

json load_project_config() {
    std::ifstream file("project.json");
    if (!file) {
        throw std::runtime_error("project.json not found.");
    }
    json config;
    file >> config;
    return config;
}

std::vector<std::string> expand_includes(const std::vector<std::string>& raw) {
    std::vector<std::string> includes;
    for (const std::string& path : raw) {
        if (path.find('*') != std::string::npos) {
            size_t pos = path.find('*');
            std::string base = path.substr(0, pos);
            if (!base.empty() && base.back() == '/') {
                base.pop_back();
            }
            try {
                for (auto& entry : fs::directory_iterator(base)) {
                    if (fs::is_directory(entry)) {
                        includes.push_back("-I" + entry.path().string());
                    }
                }
            } catch (const fs::filesystem_error& e) {
                std::cerr << "⚠️ Warning: failed to iterate directory '" << base << "': " << e.what() << "\n";
            }
        } else {
            if (fs::is_directory(path)) {
                includes.push_back("-I" + path);
            } else {
                std::cerr << "⚠️ Warning: include path '" << path << "' is not a directory. Skipping.\n";
            }
        }
    }
    return includes;
}
