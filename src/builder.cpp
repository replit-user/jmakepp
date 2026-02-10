#include "../include/dauser/builder.hpp"
#include "../include/dauser/config.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>
#include <sstream>

namespace fs = std::filesystem;

std::mutex compilation_mutex;

int run_cmd(const std::string& cmd) {
    int result = std::system((cmd + " 2>&1").c_str());
    if (result == 0) {
        return 0;
    } else {
        std::cerr << cmd << " failed with code " << result << "\n";
        return 1;
    }
}

// Compile a single source file to an object file
void compile_source(const std::string& source_file, const std::string& compiler, 
    const std::vector<std::string>& flags, 
    const std::vector<std::string>& includes,
    const std::string& output_dir, int& result
) {
    std::string base_name = fs::path(source_file).stem().string();
    std::string obj_file = output_dir + base_name + ".o";
    
    std::string command = compiler + " -c -fPIC -o \"" + obj_file + "\" \"" + source_file + "\"";
    
    for (const auto& flag : flags) {
        command += " \"" + flag + "\"";
    }
    
    for (const auto& inc : includes) {
        command += " \"" + inc + "\"";
    }
    
    {
        std::lock_guard<std::mutex> lock(compilation_mutex);
        std::cout << "🔨 Compiling: " << source_file << "\n";
    }
    
    result = run_cmd(command);
}

void build(std::string new_version) {
    json config = load_project_config();
    bool c = config["c"];
    std::string name = config["name"];
    std::string buildpath = config["buildpath"];
    std::vector<std::string> src_files = config["srcpath"].is_array() ? 
    config["srcpath"].get<std::vector<std::string>>() : 
    std::vector<std::string>{config["srcpath"].get<std::string>()};
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
    bool override_name = config["override binary name"];
    std::string override = config["binary name"];

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
        
        try {
            config["version"] = new_version;
            std::ofstream out("project.json");
            out << config.dump(4);
            std::cout << "🔄 Updated version to: " << new_version << "\n";
        } catch(std::exception) {
            std::cout << "⚠️ version not updated due to an unexpected error";
        }

        std::string platform_build_dir = buildpath + platform + "/";
        fs::create_directories(fs::path(platform_build_dir));
        
        std::vector<std::string> includes = expand_includes(includepaths);
        std::vector<std::thread> compilation_threads;
        std::vector<int> compilation_results(src_files.size(), 0);
        
        // Compile each source file in parallel
        std::cout << "📦 Starting compilation for platform: " << platform << "\n";
        for (size_t i = 0; i < src_files.size(); ++i) {
            compilation_threads.emplace_back(
                compile_source, 
                src_files[i], 
                compiler, 
                std::ref(flags), 
                std::ref(includes),
                std::ref(platform_build_dir),
                std::ref(compilation_results[i])
            );
        }
        
        // Wait for all compilation threads to complete
        for (auto& thread : compilation_threads) {
            thread.join();
        }
        
        // Check if all compilations succeeded
        bool compilation_success = true;
        for (int result : compilation_results) {
            if (result != 0) {
                compilation_success = false;
                break;
            }
        }
        
        if (!compilation_success) {
            std::cout << "❌ Build failed for platform: " << platform << " (compilation stage)\n";
            all_success = false;
            continue;
        }
        
        // Link all object files together
        std::string outname = buildpath + name + "-" + config_version + "-" + platform + extension;
        if (override_name) {
            outname = override;
        }
        
        std::string link_command = compiler + " -o \"" + outname + "\"";
        
        // Add all compiled object files
        for (const auto& src_file : src_files) {
            std::string base_name = fs::path(src_file).stem().string();
            std::string obj_file = platform_build_dir + base_name + ".o";
            link_command += " \"" + obj_file + "\"";
        }
        
        // Apply platform-specific shared library flags
        if (type == "shared") {
            if (platform == "macos") {
                link_command += " -dynamiclib";
            } else {
                link_command += " -shared";
            }
        }
        
        for (const auto& flag : flags) {
            link_command += " \"" + flag + "\"";
        }
        
        std::cout << "🔗 Linking: " << outname << "\n";
        int link_result = run_cmd(link_command);
        
        if (link_result != 0) {
            std::cout << "❌ Build failed for platform: " << platform << " (linking stage)\n";
            all_success = false;
        } else {
            std::cout << "✅ Built for " << platform << " -> " << outname << "\n";
        }
        
        // Clean up object files (main thread)
        std::cout << "🧹 Cleaning up temporary object files...\n";
        for (const auto& src_file : src_files) {
            std::string base_name = fs::path(src_file).stem().string();
            std::string obj_file = platform_build_dir + base_name + ".o";
            try {
                if (fs::exists(obj_file)) {
                    fs::remove(obj_file);
                }
            } catch (const std::exception& e) {
                std::cerr << "⚠️ Failed to delete " << obj_file << ": " << e.what() << "\n";
            }
        }
        
        // Clean up empty platform directory if it exists and is empty
        try {
            if (fs::exists(platform_build_dir) && fs::is_empty(platform_build_dir)) {
                fs::remove(platform_build_dir);
            }
        } catch (const std::exception& e) {
            std::cerr << "⚠️ Failed to clean up directory: " << e.what() << "\n";
        }
    }
}
