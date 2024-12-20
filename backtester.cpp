//
// Created by Kareem Fareed on 12/19/24.
//

#include "Backtester.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <numeric>
#include <iomanip>

Backtester::Backtester(const std::string& filePath, double capital)
    : initialCapital(capital), finalCapital(capital) {
    loadCSV(filePath);
}

// Load market data from a CSV file
void Backtester::loadCSV(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filePath << std::endl;
        return;
    }

    std::string line, date;
    double open, close;

    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::getline(ss, date, ','); // Date
        ss >> open;
        ss.ignore();
        ss >> close;
        marketData.emplace_back(date, open, close);
    }

    file.close();
}

// Generate trading signals using SMA and LMA crossover
void Backtester::generateSignals(int shortWindow, int longWindow) {
    signals.resize(marketData.size(), 0);
    std::vector<double> closes;

    for (const auto& [_, open, close] : marketData) {
        closes.push_back(close);
    }

    for (size_t i = longWindow; i < marketData.size(); ++i) {
        double sma = std::accumulate(closes.begin() + i - shortWindow, closes.begin() + i, 0.0) / shortWindow;
        double lma = std::accumulate(closes.begin() + i - longWindow, closes.begin() + i, 0.0) / longWindow;

        if (sma > lma && signals[i - 1] <= 0) {
            signals[i] = 1; // Buy
            std::cout << "Buy signal on " << std::get<0>(marketData[i]) << std::endl;
        } else if (sma < lma && signals[i - 1] >= 0) {
            signals[i] = -1; // Sell
            std::cout << "Sell signal on " << std::get<0>(marketData[i]) << std::endl;
        }
    }
}

// Simulate trading and calculate performance
void Backtester::calculatePerformance() {
    double position = 0.0;

    for (size_t i = 0; i < marketData.size(); ++i) {
        auto [_, open, close] = marketData[i];

        if (signals[i] == 1 && position == 0) {
            position = finalCapital / open; // Buy
            finalCapital = 0.0;
        } else if (signals[i] == -1 && position > 0) {
            finalCapital = position * close; // Sell
            position = 0.0;
        }

        // Track daily portfolio value
        double portfolioValue = finalCapital + (position * close);
        portfolioValues.push_back(portfolioValue);
    }

    // Add remaining position to capital
    if (position > 0) {
        finalCapital += position * std::get<2>(marketData.back());
    }
}

double Backtester::calculateSharpeRatio() const {
    if (portfolioValues.size() < 2) return 0.0;

    std::vector<double> dailyReturns;
    for (size_t i = 1; i < portfolioValues.size(); ++i) {
        double dailyReturn = (portfolioValues[i] - portfolioValues[i - 1]) / portfolioValues[i - 1];
        dailyReturns.push_back(dailyReturn);
    }

    double meanReturn = std::accumulate(dailyReturns.begin(), dailyReturns.end(), 0.0) / dailyReturns.size();
    double variance = std::accumulate(dailyReturns.begin(), dailyReturns.end(), 0.0,
        [meanReturn](double sum, double r) { return sum + (r - meanReturn) * (r - meanReturn); }) / dailyReturns.size();

    double stdDev = std::sqrt(variance);
    return (meanReturn / stdDev) * std::sqrt(252); // Annualize Sharpe Ratio (252 trading days)
}

double Backtester::calculateMaxDrawdown() const {
    double maxDrawdown = 0.0;
    double peak = portfolioValues[0];

    for (double value : portfolioValues) {
        if (value > peak) peak = value;
        double drawdown = (peak - value) / peak;
        maxDrawdown = std::max(maxDrawdown, drawdown);
    }

    return maxDrawdown * 100.0; // Convert to percentage
}

int Backtester::calculateTradingDays() const {
    return static_cast<int>(marketData.size());
}

// Run the backtester
void Backtester::run(int shortWindow, int longWindow) {
    generateSignals(shortWindow, longWindow);
    calculatePerformance();
}

// Print a summary report
void Backtester::printReport() const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Initial Capital: $" << initialCapital << std::endl;
    std::cout << "Final Capital: $" << finalCapital << std::endl;
    std::cout << "Net Return: " << ((finalCapital - initialCapital) / initialCapital) * 100 << "%" << std::endl;
    std::cout << "Annualized Return: " << (std::pow((finalCapital / initialCapital), (252.0 / calculateTradingDays())) - 1) * 100 << "%" << std::endl;
    std::cout << "Sharpe Ratio: " << calculateSharpeRatio() << std::endl;
    std::cout << "Maximum Drawdown: " << calculateMaxDrawdown() << "%" << std::endl;
}