#include "../../include/dauser/cli.hpp"
#include "../../include/dauser/builder.hpp"
#include "../../include/dauser/project.hpp"
#include "../../include/dauser/installer.hpp"
#include "../../include/dauser/updater.hpp"
#include "../../include/dauser/config.hpp"
#include "../../include/dauser/filio.hpp"
#include <iostream>
#include <filesystem>

std::string get_version() {
    return "1.9.0";
}

void show_help() {
    std::cout << "Commands:\n"
              << "  new <path>      - Creates new project\n"
              << "  build <version> - Builds project and updates version\n"
              << "  install <path>  - Installs headers from path\n"
              << "  update          - Updates the tool to the latest version\n"
              << "  clean           - Removes build directory\n"
              << "  version         - Shows current version\n"
              << "  help            - Shows this message\n";
}
