//
// Created by Kareem Fareed on 12/19/24.
//

#include <iostream>
#include <curl/curl.h>
#include <string>
#include <fstream>
#include "env_parser.h"

// Callback function for cURL
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* out) {
    size_t totalSize = size * nmemb;
    out->append((char*)contents, totalSize);
    return totalSize;
}

// Function to fetch market data from Alpha Vantage
void fetchMarketData(const std::string& symbol, const std::string& outputFile) {
    auto env = loadEnv(".env");
    auto apiKeyIter = env.find("ALPHA_VANTAGE_API_KEY");

    if (apiKeyIter == env.end()) {
        std::cerr << "Error: ALPHA_VANTAGE_API_KEY not found in .env file" << std::endl;
        return;
    }

    std::string apiKey = apiKeyIter->second;

    std::string url = "https://www.alphavantage.co/query?function=TIME_SERIES_DAILY_ADJUSTED&symbol=" +
                      symbol + "&outputsize=full&apikey=" + apiKey + "&datatype=csv";

    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);

        // Save the response to the specified file
        std::ofstream outFile(outputFile);
        if (outFile.is_open()) {
            outFile << readBuffer;
            outFile.close();
            std::cout << "Market data saved to " << outputFile << std::endl;
        } else {
            std::cerr << "Error: Could not open file " << outputFile << " for writing." << std::endl;
        }
    } else {
        std::cerr << "Failed to initialize cURL" << std::endl;
    }
}