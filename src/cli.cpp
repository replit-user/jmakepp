
#include <iostream>

std::string get_version() {
    return "2.0.2";
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
