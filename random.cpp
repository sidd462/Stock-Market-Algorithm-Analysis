#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib> // For rand() and srand()
#include <ctime> // For time()

struct MarketData {
    std::string date;
    double open;
    double high;
    double low;
    double close;
};

std::vector<MarketData> readData(const std::string& filename) {
    std::vector<MarketData> data;
    std::ifstream file(filename);
    std::string line, value;

    if (file.is_open()) {
        std::getline(file, line); // Skip CSV header
        while (std::getline(file, line)) {
            std::istringstream s(line);
            MarketData dayData;
            std::getline(s, dayData.date, ',');
            
            std::getline(s, value, ',');
            dayData.open = std::stod(value);
            
            std::getline(s, value, ',');
            dayData.high = std::stod(value);
            
            std::getline(s, value, ',');
            dayData.low = std::stod(value);
            
            std::getline(s, value, ',');
            dayData.close = std::stod(value);

            data.push_back(dayData);
        }
    } else {
        std::cerr << "Could not open the file." << std::endl;
    }
    file.close();
    return data;
}


double randomTradingStrategy(const std::vector<MarketData>& data) {
    srand(time(0)); // Seed random number generator
    double initialCapital = 10000;
    double capital = initialCapital;
    int sharesHeld = 0;
    double buyPrice = 0.0;
    int daysSinceLastAction = 10; // Start ready to buy

    for (const auto& day : data) {
        // Try to buy if not holding and days since last action >= 10
        if (!sharesHeld && daysSinceLastAction >= 10) {
            if (rand() % 2) { // Randomly decide to buy or not (50% chance)
                buyPrice = day.open;
                sharesHeld = capital / buyPrice;
                capital -= sharesHeld * buyPrice;
                std::cout << day.date << ": Bought " << sharesHeld << " shares at " << buyPrice << std::endl;
                daysSinceLastAction = 0; // Reset counter
            }
        }

        if (sharesHeld) {
            double currentPrice = day.close;
            double profitLossPercent = (currentPrice - buyPrice) / buyPrice * 100;

            // Sell if loss > 2% or profit > 1%
            if (profitLossPercent < -2 || profitLossPercent > 1) {
                capital += sharesHeld * currentPrice;
                std::cout << day.date << ": Sold " << sharesHeld << " shares at " << currentPrice 
                          << " (" << (profitLossPercent > 0 ? "Profit" : "Loss") 
                          << " of " << profitLossPercent << "%)" << std::endl;
                sharesHeld = 0; // Reset shares
                daysSinceLastAction = 0; // Reset counter
            }
        } else {
            daysSinceLastAction++; // Increment days since last action if not holding shares
        }
    }

    // Final capital calculation
    std::cout << "Final Capital: " << capital << std::endl;
    return capital;
}

int main() {
    std::string dataFile = "nasdaq50.csv"; // Replace with the actual data file path
    std::vector<MarketData> historicalData = readData(dataFile);
    std::vector<double> finalCapitals;


    if (!historicalData.empty()) {
        for (int i = 0; i < 10; ++i) {
        double finalCapital = randomTradingStrategy(historicalData);
        finalCapitals.push_back(finalCapital);
    }
    
    } else {
        std::cerr << "Historical data is empty." << std::endl;
    }
    double sum = 0;
    for (double capital : finalCapitals) {
        sum += capital;
    }
    double average = sum / finalCapitals.size();
    std::cout<<"Average final capital: "<<average<<std::endl;
    std::ofstream outFile("final_capitals.csv");
    if (outFile.is_open()) {
        outFile << "Run,FinalCapital\n";
        for (size_t i = 0; i < finalCapitals.size(); ++i) {
            outFile << i + 1 << "," << finalCapitals[i] << "\n";
        }
        outFile << "Average," << average << "\n";
        outFile.close();
    } else {
        std::cerr << "Could not open the output file." << std::endl;
    }


    return 0;
}
