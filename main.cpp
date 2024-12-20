#include <iostream>
#include <fstream>
#include <filesystem>
#include <ctime>
#include "dataloader.h"
#include "backtester.h"

// Function to check if a file is up-to-date
bool isFileUpToDate(const std::string& filePath) {
    if (!std::filesystem::exists(filePath)) {
        return false;
    }

    auto lastWriteTime = std::filesystem::last_write_time(filePath);

    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        lastWriteTime - std::filesystem::file_time_type::clock::now() +
        std::chrono::system_clock::now()
    );

    std::time_t lastWriteTimeT = std::chrono::system_clock::to_time_t(sctp);

    std::time_t now = std::time(nullptr);
    std::tm today = *std::localtime(&now);
    today.tm_hour = 0;
    today.tm_min = 0;
    today.tm_sec = 0;
    std::time_t todayStart = std::mktime(&today);

    // Check if the file was modified today
    return lastWriteTimeT >= todayStart;
}

int main() {
    std::string symbol;
    double initialCapital;
    int sma_length, lma_length;
    std::cout << "Enter stock symbol: ";
    std::cin >> symbol;
    std::cout << "Enter starting capital: ";
    std::cin >> initialCapital;
    std::cout << "Choose an SMA length: ";
    std::cin >> sma_length;
    std::cout << "Choose an LMA length: ";
    std::cin >> lma_length;

    std::string outputFile = "stocks/" + symbol + ".csv";

    // Check if the file exists and is up-to-date
    if (isFileUpToDate(outputFile)) {
        std::cout << "The file for " << symbol << " is up-to-date. No API call needed." << std::endl;
    } else {
        std::cout << "Fetching market data for " << symbol << "..." << std::endl;
        fetchMarketData(symbol, outputFile);
    }
    std::string filePath = "stocks/" + symbol + ".csv";
    Backtester backtester(filePath, initialCapital);
    backtester.run(sma_length, lma_length);
    backtester.printReport();
    return 0;
}