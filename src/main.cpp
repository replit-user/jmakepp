#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <regex>
#include <sstream>
#include "../include/nlohmann/json.hpp" // JSON library
#include "./filio.hpp" // simpler file I/O
#ifdef _WIN32
    bool is_windows = true;
#else
    bool is_windows = false;
#endif
#include <filesystem>
namespace fs = std::filesystem;

using json = nlohmann::json;
std::string version = "1.8.4";
// Utility to run a system command and print it
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

// Load JSON config from file
json load_project_config() {
    std::ifstream file("project.json");
    if (!file) {
        throw std::runtime_error("project.json not found.");
    }
    json config;
    file >> config;
    return config;
}

// Expand wildcard include paths (supports simple * wildcard)
std::vector<std::string> expand_includes(const std::vector<std::string>& raw) {
    std::vector<std::string> includes;
    for (const std::string& path : raw) {
        if (path.find('*') != std::string::npos) {
            size_t pos = path.find('*');
            std::string base = path.substr(0, pos);
            // Remove trailing slash if exists
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

// Build project
void build(std::string new_version) {
    json config = load_project_config();
    std::string name = config["name"];
    std::string buildpath = config["buildpath"];
    std::string src = config["srcpath"];
    std::string type = config["type"];
    std::vector<std::string> includepaths = config.value("includepaths", std::vector<std::string>{"./include/*"});
    std::string config_version = config["version"];
    std::vector<std::string> flags;

    // Support both array and string format for flags
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

    for (const std::string& platform : platforms) {
        std::string compiler, extension;
        if (platform == "linux") {
            compiler = "g++";
            extension = "";
        } else if (platform == "windows") {
            compiler = "x86_64-w64-mingw32-g++";
            extension = ".exe";
        } else {
            std::cerr << "⚠️ Unsupported platform: " << platform << "\n";
            continue;
        }

        std::string outname = buildpath + name + "-" + config_version + "-" + platform + extension;
        std::string command = compiler + " -o \"" + outname + "\" \"" + src + "\"";

        for (const auto& flag : flags) {
            command += " \"" + flag + "\"";
        }

        if (type == "shared") {
            command += " -shared -fPIC";
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


// Create new project directory
void create_new_project(const std::string& path) {
    fs::create_directories(path + "/src");
    fs::create_directories(path + "/include");

    std::ofstream proj(path + "/project.json");
    proj << R"({
  "flags":"-std=c++17",
  "buildpath": "./build/",
  "includepaths": ["./include/*"],
  "srcpath": "./src/main.cpp",
  "version": "1.0",
  "type": "elf",
  "name": "example",
  "platforms": ["linux","windows"]
})";
    proj.close();

    std::string abs_path = fs::absolute(fs::path(path)).string();
    // Escape quotes in path
    size_t pos = 0;
    while ((pos = abs_path.find('"', pos)) != std::string::npos) {
        abs_path.replace(pos, 1, "\\\"");
        pos += 2;
    }
    std::ofstream maincpp(path + "/src/main.cpp");
    maincpp << "#include <iostream>\nint main() {\n    std::cout << \"Hello from " << abs_path << "!\\n\";\n    return 0;\n}\n";
    maincpp.close();

    std::cout << "✅ Project created at: " << path << "\n";
}

// Install header files to ./include/
void install_headers(const std::string& from_path) {
    fs::create_directories("include");
    fs::path source = fs::absolute(from_path);

    if (!fs::exists(source)) {
        std::cout << "❌ Error: source path does not exist.\n";
        return;
    }

    // Handle single file
    if (fs::is_regular_file(source)) {
        if (source.extension() == ".h" || source.extension() == ".hpp") {
            fs::copy_file(source,
                          fs::path("include") / source.filename(),
                          fs::copy_options::overwrite_existing);
            std::cout << "✅ Installed header: " << source.filename() << "\n";
        } else {
            std::cout << "❌ Not a header file: " << source << "\n";
        }
        return;
    }

    // Handle directory
    for (auto& entry : fs::recursive_directory_iterator(source)) {
        if (entry.is_regular_file() &&
            (entry.path().extension() == ".h" || entry.path().extension() == ".hpp")) {

            // Compute relative path
            auto rel_path = fs::relative(entry.path().parent_path(), source);
            fs::path dest = fs::path("include") / rel_path / entry.path().filename();

            fs::create_directories(dest.parent_path());
            fs::copy_file(entry.path(), dest, fs::copy_options::overwrite_existing);
        }
    }

    std::cout << "✅ Headers installed to ./include\n";
}

// Update the tool to the latest version from GitHub
void update(std::string final_dest) {
    std::string url = "https://github.com/replit-user/jmakepp.git";
    std::string dest = "./TMP";
    std::string cmd = "git clone " + url + " " + dest;

    if (run_cmd(cmd) != 0) {
        std::cerr << "❌ Failed to clone repository.\n";
        return;
    }

    std::string binary_name = is_windows ? "jmakepp.exe" : "jmakepp";
    std::string src = dest + "/bin/" + binary_name;

    if (!fs::exists(src)) {
        std::cerr << "❌ Binary not found in repository.\n";
        run_cmd(is_windows ? "rmdir /s /q TMP" : "rm -rf ./TMP");
        return;
    }

    try {
        fs::copy_file(src, final_dest, fs::copy_options::overwrite_existing);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "❌ Failed to update binary: " << e.what() << "\n";
        run_cmd(is_windows ? "rmdir /s /q TMP" : "rm -rf ./TMP");
        return;
    }

    if (!is_windows) {
        run_cmd("chmod +x \"" + final_dest + "\"");
    }

    run_cmd(is_windows ? "rmdir /s /q TMP" : "rm -rf ./TMP");

    std::cout << "✅ Update completed successfully.\n";
}



// Main CLI dispatcher
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: jmakepp [build|new|install|help|version|clean|update] [optional args]\n";
        return 1;
    }

    std::string cmd = argv[1];
    std::string exe_path = argv[0];

    try {
        if (cmd == "build") {
            if (argc < 3) {
                std::cout << "Usage: jmakepp build <new_version>\n";
                return 1;
            }
            build(argv[2]);
        } else if (cmd == "new") {
            if (argc < 3) {
                std::cout << "Usage: jmakepp new <path>\n";
                return 1;
            }
            create_new_project(argv[2]);
        } else if (cmd == "install") {
            if (argc < 3) {
                std::cout << "Usage: jmakepp install <path>\n";
                return 1;
            }
            install_headers(argv[2]);
        } else if (cmd == "help") {
            std::cout << "Commands:\n"
                      << "  new <path>      - Creates new project\n"
                      << "  build <version> - Builds project and updates version\n"
                      << "  install <path>  - Installs headers from path\n"
                      << "  update          - Updates the tool to the latest version\n"
                      << "  help            - Shows this message\n";
        } else if(cmd == "version") {
            std::cout << "Version: " << version << "\n";
            return 0;
        } else if (cmd == "clean") {
            std::string buildpath = load_project_config()["buildpath"];
            if (fs::exists(buildpath)) {
                fs::remove_all(buildpath);
                std::cout << "✅ Cleaned build directory\n";
            } else {
                std::cout << "⚠️ Build directory does not exist\n";
            }
        } else if (cmd == "update") {
            update(filio::extra::script_path().string());
        }
        else {
            std::cout << "❌ Unknown command: " << cmd << "\n";
            return 1;
        }
    } catch (std::exception& e) {
        std::cerr << "❌ Exception: " << e.what() << "\n";
        return 1;
    }

    return 0;
}