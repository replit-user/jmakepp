#include "../../include/dauser/installer.hpp"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

void install_headers(const std::string& from_path) {
    fs::create_directories("include");
    fs::path source = fs::absolute(from_path);

    if (!fs::exists(source)) {
        std::cout << "❌ Error: source path does not exist.\n";
        return;
    }

    if (fs::is_regular_file(source)) {
        if (source.extension() == ".h" || source.extension() == ".hpp") {
            fs::copy_file(source, fs::path("include") / source.filename(), fs::copy_options::overwrite_existing);
            std::cout << "✅ Installed header: " << source.filename() << "\n";
        } else {
            std::cout << "❌ Not a header file: " << source << "\n";
        }
        return;
    }

    for (auto& entry : fs::recursive_directory_iterator(source)) {
        if (entry.is_regular_file() && (entry.path().extension() == ".h" || entry.path().extension() == ".hpp")) {
            auto rel_path = fs::relative(entry.path().parent_path(), source);
            fs::path dest = fs::path("include") / rel_path / entry.path().filename();
            fs::create_directories(dest.parent_path());
            fs::copy_file(entry.path(), dest, fs::copy_options::overwrite_existing);
        }
    }
    std::cout << "✅ Headers installed to ./include\n";
}
