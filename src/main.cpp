#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <regex>

#include "../include/nlohmann/json.hpp" // JSON library
#include "../include/dauser/filio.h" // simpler file I/O

#include <filesystem>
namespace fs = std::filesystem;

using json = nlohmann::json;
std::string version = "1.7.0";
// Utility to run a system command and print it
int run_cmd(const std::string& cmd) {
    std::cout << "🚧 Running: " << cmd << "\n";
    return std::system(cmd.c_str());
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
            for (auto& entry : fs::directory_iterator(base)) {
                if (fs::is_directory(entry)) {
                    includes.push_back("-I" + entry.path().string());
                }
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
    std::string version = config["version"];

    fs::create_directories(fs::path(buildpath));

    std::vector<std::string> platforms = config.value("platforms", std::vector<std::string>{"linux"});

for (const std::string& platform : platforms) {
    std::string compiler;
    std::string extension;
    if (platform == "linux") {
        compiler = "g++";
        extension = "";
    } else if (platform == "windows") {
        compiler = "x86_64-w64-mingw32-g++";  // Linux-to-Windows cross-compiler
        extension = ".exe";
    } else {
        std::cerr << "⚠️ Unsupported platform: " << platform << "\n";
        continue;
    }

    std::string outname = buildpath + name + "-" + version + "-" + platform + extension;
    std::string command = compiler + " -o " + outname + " " + src;

    if (type == "shared") command.append(" -shared -fPIC");

    std::vector<std::string> includes = expand_includes(includepaths);
    for (const auto& inc : includes) command += " " + inc;

    int result = run_cmd(command);
    if (result != 0) {
        std::cout << "❌ Build failed for platform: " << platform << "\n";
    } else {
        std::cout << "✅ Built for " << platform << " -> " << outname << "\n";
    }
}config["version"] = new_version;
std::ofstream out("project.json");
out << config.dump(4);
std::cout << "🔄 Updated version to: " << new_version << "\n";

}

// Create new project directory
void create_new_project(const std::string& path) {
    fs::create_directories(path + "/src");
    fs::create_directories(path + "/include");

    std::ofstream proj(path + "/project.json");
    proj << R"({
  "buildpath": "./build/",
  "includepaths": ["./include/*"],
  "srcpath": "./src/main.cpp",
  "version": "1.0",
  "type": "elf",
  "name": "example",
  "platforms": ["linux","windows"]
})";
    proj.close();

    std::ofstream maincpp(path + "/src/main.cpp");
    maincpp << "#include <iostream>\nint main() {\n    std::cout << \"Hello from " << path << "!\\n\";\n    return 0;\n}\n";
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

// Main CLI dispatcher
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: sbuild [build|new|install|help] [optional args]\n";
        return 1;
    }

    std::string cmd = argv[1];

    try {
        if (cmd == "build") {
            if (argc < 3) {
                std::cout << "Usage: sbuild build <new_version>\n";
                return 1;
            }
            build(argv[2]);
        } else if (cmd == "new") {
            if (argc < 3) {
                std::cout << "Usage: sbuild new <path>\n";
                return 1;
            }
            create_new_project(argv[2]);
        } else if (cmd == "install") {
            if (argc < 3) {
                std::cout << "Usage: sbuild install <path>\n";
                return 1;
            }
            install_headers(argv[2]);
        } else if (cmd == "help") {
            std::cout << "Commands:\n"
                      << "  new <path>      - Creates new project\n"
                      << "  build <version> - Builds project and updates version\n"
                      << "  install <path>  - Installs headers from path\n"
                      << "  help            - Shows this message\n";
        } else if(cmd == "version") {
            std::cout << "Version: " << version << "\n";
            return 0;
        }else if(cmd == "clean"){
            fs::remove_all(load_project_config()["buildpath"]);
            std::cout << "✅ Cleaned build directory\n";
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