#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include "../nlohmann/json.hpp"

using json = nlohmann::json;

// Load JSON config from file
json load_project_config();

// Expand wildcard include paths (supports simple * wildcard)
std::vector<std::string> expand_includes(const std::vector<std::string>& raw);

#endif // CONFIG_HPP
