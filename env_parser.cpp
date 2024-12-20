//
// Created by Kareem Fareed on 12/19/24.
//

#include "env_parser.h"
#include <sstream>
#include <iostream>

std::map<std::string, std::string> loadEnv(const std::string& filepath) {
    std::map<std::string, std::string> env;
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open .env file at " << filepath << std::endl;
        return env;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string key, value;

        if (std::getline(ss, key, '=') && std::getline(ss, value)) {
            env[key] = value;
        }
    }

    return env;
}