#include <iostream>
#include "../include/dauser/config.hpp"
#include "../include/dauser/builder.hpp"
#include "../include/dauser/installer.hpp"
#include "../include/dauser/project.hpp"
#include "../include/dauser/updater.hpp"
#include "../include/dauser/filio.hpp"
#include "../include/dauser/cli.hpp"
#include "../include/dauser/platform.hpp"
#include <vector>

std::vector<void*> allocations;

void* tracked_malloc(size_t size) {
    void* ptr = malloc(size);
    allocations.push_back(ptr);
    return ptr;
}

void free_all() {
    for (void* ptr : allocations) free(ptr);
    allocations.clear();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: jmakepp [build|new|install|help|version|clean|update] [optional args]\n";
        return 1;
    }

    std::string cmd = argv[1];

    try {
        if (cmd == "build") {
            if (argc < 3) {
                std::cout << "Usage: jmakepp build <new_version>";
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
            show_help();
        } else if(cmd == "version") {
            std::cout << "Version: " << get_version() << "\n";
            return 0;
        } else if (cmd == "clean") {
            json config = load_project_config();
            std::string buildpath = config["buildpath"];
            std::filesystem::path buildpath_fs = buildpath;
            if (std::filesystem::exists(buildpath_fs)) {
                std::filesystem::remove_all(buildpath_fs);
                std::cout << "✅ Cleaned build directory\n";
            } else if(cmd == "run"){
                json config = load_project_config();
                build(config["version"]);
                if(config["override binary name"]){
                    std::string buildpath = config["buildpath"];
                    std::string binary_name = config["binary name"];
                    run_cmd(buildpath + "" + binary_name + (is_windows?".exe":is_macos?"-macos":"-linux"));
                }else{
                    std::string version = config["version"];
                    std::string buildpath = config["buildpath"];
                    std::string name = config["name"];
                    run_cmd(buildpath + name + "-" + version + (is_windows?"-windows.exe":is_macos?"-macos":"-linux"));
                }
            }
            
            else {
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
    free_all();
    return 0;
}
