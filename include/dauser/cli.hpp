#ifndef CLI_HPP
#define CLI_HPP

#include <string>

// Get the current version
std::string get_version();

// Display help message
void show_help();

// Handle CLI commands
int handle_command(int argc, char* argv[]);

#endif // CLI_HPP
