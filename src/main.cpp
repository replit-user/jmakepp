#include <exception>
#include <iostream>
#include <string>
#include "../include/dauser/config.hpp"
#include "../include/dauser/builder.hpp"
#include "../include/dauser/installer.hpp"
#include "../include/dauser/project.hpp"
#include "../include/dauser/updater.hpp"
#include "../include/dauser/filio.hpp"
#include "../include/dauser/cli.hpp"
#include "../include/dauser/platform.hpp"
#include "../include/dauser/cmd.hpp"
#include <filesystem>
namespace fs = std::filesystem;


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: jmakepp [build|new|install|help|version|clean|update] [optional args]\n";
        return 1;
    }

    std::string cmd = argv[1];


        if (cmd == "build") {
            try{if(argc == 2){
                build("");
            }else if(argc == 1 || argc > 3){
                show_help();
            }else{
                build(argv[argc - 1]);
            }}catch(std::exception err){
                std::cerr << err.what();
                return 1;
            }
        }else if (cmd == "install") {
            if (argc < 3) {
                std::cout << "Usage: jmakepp install <path>\n";
                return 1;
            }
            install_headers(argv[2]);
        }else if (cmd == "new") {
        if (argc < 3) {
            std::cout << "Usage: jmakepp new <path>\n";
            return 1;
        }
        create_new_project(argv[2]);
    }
        else if (cmd == "help") {
            show_help();
        } else if(cmd == "version") {
            std::cout << "Version: " << get_version() << "\n";
            return 0;
        } else if (cmd == "clean") {
            if(!std::filesystem::exists("project.json")){
                std::cerr << "⚠️ project.json not found.";
                return 1;
            }
            json config = load_project_config();
            std::string buildpath = config["buildpath"];
            std::filesystem::path buildpath_fs = buildpath;
            if (std::filesystem::exists(buildpath_fs) && buildpath != "") {
                fs::remove_all(buildpath);
                std::cout << "✅ Cleaned build directory\n";
            }

            else {
                std::cerr << "⚠️ Build directory does not exist\n";
            }
        } else if (cmd == "update") {
            update(filio::extra::script_path().string());
        }else if(cmd == "run"){
                json config = load_project_config();
                build(config["version"]);
                if(config["override binary name"]){
                    std::string buildpath = config["buildpath"];
                    std::string binary_name = config["binary name"];
                    bool override_binary_name = config["override binary name"];
                    run_cmd(buildpath + "/" + binary_name + (is_windows?".exe":"_linux"));
                }else{
                    std::string version = config["version"];
                    std::string buildpath = config["buildpath"];
                    std::string name = config["name"];
                    run_cmd(buildpath + name + "-" + version + (is_windows?"-windows.exe":is_macos?"-macos":"-linux"));
                }
            }else {
            std::cout << "❌ Unknown command: " << cmd << "\n";
            return 1;
        }
    return 0;
}
