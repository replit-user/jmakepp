#include "../include/dauser/project.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

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
  "platforms": ["linux","windows"],
  "c":false,
  "override binary name":false,
  "binary name":""
})";
    proj.close();

    std::string abs_path = fs::absolute(fs::path(path)).string();
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
