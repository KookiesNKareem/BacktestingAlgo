//
// Created by Kareem Fareed on 12/19/24.
//

#ifndef ENV_PARSER_H
#define ENV_PARSER_H

#include <string>
#include <fstream>
#include <map>

// Function to load environment variables from a .env file
std::map<std::string, std::string> loadEnv(const std::string& filepath);

#endif // ENV_PARSER_H