//
// Created by Kareem Fareed on 12/19/24.
//

#ifndef BACKTESTER_H
#define BACKTESTER_H

#include <string>
#include <vector>
#include <tuple>

class Backtester {
private:
    std::vector<std::tuple<std::string, double, double>> marketData; // Date, Open, Close
    std::vector<int> signals; // Buy (1), Hold (0), Sell (-1)
    double initialCapital;
    double finalCapital;
    std::vector<double> portfolioValues;

    void loadCSV(const std::string& filePath);
    void generateSignals(int shortWindow, int longWindow);
    void calculatePerformance();
    double calculateSharpeRatio() const;
    double calculateMaxDrawdown() const;
    int calculateTradingDays() const;

public:
    Backtester(const std::string& filePath, double capital);
    void run(int shortWindow = 10, int longWindow = 50);
    void printReport() const;
};

#endif // BACKTESTER_HER_H