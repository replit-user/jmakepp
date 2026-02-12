#include "../include/dauser/updater.hpp"
#include "../include/dauser/builder.hpp"
#include "../include/dauser/filio.hpp"
#include "../include/dauser/platform.hpp"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

void update(std::string final_dest) {
    std::string url = "https://github.com/replit-user/jmakepp.git";
    std::string dest = "./TMP";
    std::string cmd = "git clone " + url + " " + dest;

    if (run_cmd(cmd) != 0) {
        std::cerr << "❌ Failed to clone repository.\n";
        return;
    }

    std::string binary_name;
    if (is_windows) {
        binary_name = "jmakepp.exe";
    } else if(is_macos) {
        binary_name = "jmakepp_macos";
    } else {
        binary_name = "jmakepp_linux";
    }
    
    std::string src = dest + "/bin/" + binary_name;

    if (!fs::exists(src)) {
        std::cerr << "❌ Binary not found in repository.\n";
        run_cmd(is_windows ? "rmdir /s /q TMP" : "rm -rf ./TMP");
        return;
    }
    try {
        std::string script_path =
            filio::extra::absolute_path(
                filio::extra::script_path().string()
            ).string();

        std::string newpath = script_path + ".old";

        fs::rename(script_path, newpath);
        fs::copy_file(src, final_dest, fs::copy_options::overwrite_existing);

    } catch (const fs::filesystem_error& e) {

        // Permission denied → retry with sudo on Unix-like systems
        if (!is_windows && e.code() == std::errc::permission_denied) {
            std::cerr << "⚠️ Permission denied, retrying with sudo...\n";

            std::string script_path =
                filio::extra::absolute_path(
                    filio::extra::script_path().string()
                ).string();

            std::string newpath = script_path + ".old";

            std::string cmd =
                "sudo mv \"" + script_path + "\" \"" + newpath + "\" && "
                "sudo cp \"" + src + "\" \"" + final_dest + "\"";

            if (std::system(cmd.c_str()) == 0) {
                std::cerr << "✅ Update succeeded with sudo\n";
                run_cmd("rm -rf ./TMP");
                return;
            } else {
                std::cerr << "❌ Sudo retry failed\n";
            }
        }

        // Normal failure path
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
