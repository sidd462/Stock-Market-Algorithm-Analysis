#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath> 

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
        std::getline(file, line);

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
double calculateSMA(const std::vector<double>& closingPrices, size_t start, size_t end) {
    double sum = 0.0;
    for (size_t i = start; i < end; i++) {
        sum += closingPrices[i];
    }
    return sum / (end - start);
}

double calculateSD(const std::vector<double>& data, double mean, size_t start, size_t end) {
    double standardDeviation = 0.0;
    for (size_t i = start; i < end; i++) {
        standardDeviation += std::pow(data[i] - mean, 2);
    }
    return std::sqrt(standardDeviation / (end - start));
}

void backtest(const std::vector<MarketData>& historicalData, int window) {
    double initialCapital = 10000.0;
    double capital = initialCapital;
    int sharesHeld = 0;
    bool positionOpen = false;

    std::vector<double> closingPrices;
    for (const auto& dayData : historicalData) {
        closingPrices.push_back(dayData.close);
    }

    std::cout << "Total data points: " << closingPrices.size() << std::endl;

    for (size_t i = window; i < closingPrices.size(); i++) {
        double mean = calculateSMA(closingPrices, i - window, i);
        double sd = calculateSD(closingPrices, mean, i - window, i);
        double upperBand = mean + 2 * sd;
        double lowerBand = mean - 2 * sd;

        std::cout << "Date: " << historicalData[i].date
                  << ", Close: " << closingPrices[i]
                  << ", Mean: " << mean
                  << ", SD: " << sd
                  << ", Upper Band: " << upperBand
                  << ", Lower Band: " << lowerBand << std::endl;

        if (closingPrices[i] < lowerBand && !positionOpen) {
            sharesHeld = capital / closingPrices[i];
            capital -= sharesHeld * closingPrices[i];
            positionOpen = true;
            std::cout << historicalData[i].date << ": Buy " << sharesHeld << " shares at " << closingPrices[i] << std::endl;
        }
        else if (closingPrices[i] > upperBand && positionOpen) {
            capital += sharesHeld * closingPrices[i];
            positionOpen = false;
            sharesHeld = 0;
            std::cout << historicalData[i].date << ": Sell " << sharesHeld << " shares at " << closingPrices[i] << std::endl;
            std::cout << "Current Capital: " << capital << std::endl;
        }
    }

    if (positionOpen) {
        capital += sharesHeld * closingPrices.back();
        std::cout << "Final position closed at " << closingPrices.back() << std::endl;
        std::cout << "Final Capital: " << capital << std::endl;
        positionOpen = false;
        sharesHeld = 0;
    }
}

int main() {
    std::string dataFile = "NASDAQ.csv"; 
    std::vector<MarketData> historicalData = readData(dataFile);
    if (!historicalData.empty()) {
        int window = 20; 
        if (historicalData.size() >= window) {
            backtest(historicalData, window);
        } else {
            std::cerr << "Not enough data points for the specified window." << std::endl;
        }
    } else {
        std::cerr << "Historical data is empty." << std::endl;
    }
    return 0;
}
