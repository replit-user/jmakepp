#ifndef BUILDER_HPP
#define BUILDER_HPP

#include <string>
#include <vector>

// Utility to run a system command and print it
int run_cmd(const std::string& cmd);

// Build project with new version
void build(std::string new_version);

#endif // BUILDER_HPP
