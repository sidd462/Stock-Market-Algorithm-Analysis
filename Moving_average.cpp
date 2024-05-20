#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

// Define a data structure to hold one day's market data
struct MarketData {
    std::string date;
    double open;
    double high;
    double low;
    double close;
};

// Function to read market data from a CSV file
std::vector<MarketData> readData(const std::string& filename) {
    std::vector<MarketData> data;
    std::ifstream file(filename);
    std::string line, word;

    if (file.is_open()) {
        // Skip CSV header
        std::getline(file, line);

        // Read each line
        while (std::getline(file, line)) {
            std::istringstream s(line);
            MarketData dayData;
            std::getline(s, word, ','); // Date
            dayData.date = word;
            std::getline(s, word, ','); // Open
            dayData.open = std::stod(word);
            std::getline(s, word, ','); // High
            dayData.high = std::stod(word);
            std::getline(s, word, ','); // Low
            dayData.low = std::stod(word);
            std::getline(s, word, ','); // Close
            dayData.close = std::stod(word);

            
            data.push_back(dayData);
        }
    } else {
        std::cerr << "Could not open the file." << std::endl;
    }

    file.close();
    return data;
}

// Calculate Simple Moving Average (SMA)
double calculateSMA(const std::vector<double>& closingPrices, size_t start, size_t end) {
    double sum = 0.0;
    for (size_t i = start; i < end; i++) {
        sum += closingPrices[i];
    }
    return sum / (end - start);
}

// Main backtesting function
void backtest(const std::vector<MarketData>& historicalData, int shortWindow, int longWindow) {
    double initialCapital = 10000.0;
    double capital = initialCapital;
    int sharesHeld = 0; // New variable to track the number of shares held
    bool positionOpen = false;

    std::vector<double> closingPrices;
    for (const auto& dayData : historicalData) {
        closingPrices.push_back(dayData.close);
    }

    for (size_t i = longWindow; i < closingPrices.size(); i++) {
        double shortMA = calculateSMA(closingPrices, i - shortWindow, i);
        double longMA = calculateSMA(closingPrices, i - longWindow, i);

        if (shortMA > longMA && !positionOpen) {
            // Calculate the number of shares to buy based on available capital and stock price
            sharesHeld = capital / closingPrices[i];
            positionOpen = true;
            capital -= sharesHeld * closingPrices[i]; // Update capital to reflect the purchase
            std::cout << historicalData[i].date << ": Buy " << sharesHeld << " shares at " << closingPrices[i] << std::endl;
        } else if (shortMA < longMA && positionOpen) {
            // Sell all held shares
            capital += sharesHeld * closingPrices[i]; // Update capital to reflect the sale
            std::cout << historicalData[i].date << ": Sell " << sharesHeld << " shares at " << closingPrices[i] << std::endl;
            std::cout << "Current Capital: " << capital << std::endl;
            positionOpen = false;
            sharesHeld = 0; // Reset shares held
        }
    }

    // If position remains open at the end, close it
    if (positionOpen && sharesHeld > 0) {
        capital += sharesHeld * closingPrices.back(); // Sell any remaining shares at the last price
        std::cout << "Final position closed at " << closingPrices.back() << std::endl;
        std::cout << "Final Capital: " << capital << std::endl;
        positionOpen = false;
        sharesHeld = 0; // Reset shares held
    }
}
int main() {
    std::string dataFile = "NASDAQ50.csv"; // Replace with your actual data file path
    std::vector<MarketData> historicalData = readData(dataFile);

    if (!historicalData.empty()) {
        int shortWindow = 50; // Short-term moving average window
        int longWindow = 200; // Long-term moving average window

        // Ensure we have enough data points for the long window
        if (historicalData.size() >= longWindow) {
            backtest(historicalData, shortWindow, longWindow);
        } else {
            std::cerr << "Not enough data points for the specified windows." << std::endl;
        }
    } else {
        std::cerr << "Historical data is empty." << std::endl;
    }

    return 0;
}
