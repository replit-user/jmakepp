#include "../../include/dauser/builder.hpp"
#include "../../include/dauser/config.hpp"
#include <iostream>
#include <sstream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

int run_cmd(const std::string& cmd) {
    std::cout << "🚧 Running: " << cmd << "\n";
    int result = std::system(cmd.c_str());
    if (result == 0) {
        return 0;
    } else {
        std::cerr << cmd << " failed with code " << result << "\n";
        return 1;
    }
}

void build(std::string new_version) {
    json config = load_project_config();
    bool c = config["c"];
    std::string name = config["name"];
    std::string buildpath = config["buildpath"];
    std::string src = config["srcpath"];
    std::string type = config["type"];
    std::vector<std::string> includepaths = config.value("includepaths", std::vector<std::string>{"./include/*"});
    std::string config_version = config["version"];
    std::vector<std::string> flags;

    if (config["flags"].is_string()) {
        std::istringstream iss(config["flags"].get<std::string>());
        std::string flag;
        while (iss >> flag) flags.push_back(flag);
    } else if (config["flags"].is_array()) {
        flags = config["flags"].get<std::vector<std::string>>();
    }

    fs::create_directories(fs::path(buildpath));
    std::vector<std::string> platforms = config.value("platforms", std::vector<std::string>{"linux"});
    bool all_success = true;

#ifdef _WIN32
    bool is_windows = true;
    bool is_macos = false;
#elif __APPLE__
    bool is_windows = false;
    bool is_macos = true;
#else
    bool is_windows = false;
    bool is_macos = false;
#endif

    for (const std::string& platform : platforms) {
        std::string compiler, extension;
        
        if (platform == "linux") {
            compiler = c ? "gcc" : "g++";
            extension = (type == "shared") ? ".so" : "";
        }
        else if (platform == "windows") {
            compiler = c ? "x86_64-w64-mingw32-gcc" : "x86_64-w64-mingw32-g++";
            extension = (type == "shared") ? ".dll" : ".exe";
        }
        else if (platform == "macos") {
            compiler = c ? "clang" : "clang++";
            extension = (type == "shared") ? ".dylib" : "";
        }
        else {
            std::cerr << "⚠️ Unsupported platform: " << platform << "\n";
            continue;
        }

        std::string outname = buildpath + name + "-" + config_version + "-" + platform + extension;
        std::string command = compiler + " -o \"" + outname + "\" \"" + src + "\"";

        for (const auto& flag : flags) {
            command += " \"" + flag + "\"";
        }

        // Apply platform-specific shared library flags
        if (type == "shared") {
            if (platform == "macos") {
                command += " -dynamiclib";
            } else {
                command += " -shared -fPIC";
            }
        }

        std::vector<std::string> includes = expand_includes(includepaths);
        for (const auto& inc : includes) {
            command += " \"" + inc + "\"";
        }

        int result = run_cmd(command);
        if (result != 0) {
            std::cout << "❌ Build failed for platform: " << platform << "\n";
            all_success = false;
        } else {
            std::cout << "✅ Built for " << platform << " -> " << outname << "\n";
        }
    }

    if (all_success) {
        config["version"] = new_version;
        std::ofstream out("project.json");
        out << config.dump(4);
        std::cout << "🔄 Updated version to: " << new_version << "\n";
    } else {
        std::cout << "⚠️ Version not updated due to build failures\n";
    }
}
