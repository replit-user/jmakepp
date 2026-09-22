#include "../include/dauser/cmd.hpp"
#include <iostream>
#include <string>
int run_cmd(const std::string& cmd) {
    int result = std::system((cmd + " 2>&1").c_str());
    if (result == 0) {
        return 0;
    } else {
        std::cerr << cmd << " failed with code " << result << "\n";
        return 1;
    }
}
