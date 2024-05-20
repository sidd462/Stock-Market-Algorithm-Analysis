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
    double initialCapital = 10000; // Start with an initial capital
    int sharesHeld = 0;
    double buyPrice = 0.0;

    // Iterate through each day's market data
    for (const auto& day : data) {
        // Buy only if we haven't bought yet
        if (!sharesHeld) {
            buyPrice = day.open; // Assume buying at the open price
            sharesHeld = initialCapital / buyPrice; // Calculate the number of shares to buy
            initialCapital = 0; // All capital is invested
            std::cout << day.date << ": Bought " << sharesHeld << " shares at " << buyPrice << std::endl;
            continue; // Skip the rest of the loop after buying
        }
        
        // If shares are held, check for sell conditions
        if (sharesHeld) {
            double currentPrice = day.close; // Use the close price for selling
            double profitLossPercent = (currentPrice - buyPrice) / buyPrice;

            // Sell if profit is 2% or loss is 1%
            if (profitLossPercent >= 0.02 || profitLossPercent <= -0.01) {
                initialCapital = sharesHeld * currentPrice; // Calculate the new capital after selling
                std::cout << day.date << ": Sold " << sharesHeld << " shares at " << currentPrice 
                          << ", " << (profitLossPercent >= 0 ? "Profit" : "Loss") 
                          << " of " << profitLossPercent * 100 << "%" << std::endl;
                sharesHeld = 0; // No shares held after selling
                break; // Exit the loop after selling
            }
        }
    }

    // If still holding shares at the end of the data, calculate the final capital
    if (sharesHeld > 0) {
        double finalPrice = data.back().close; // Use the last available close price
        initialCapital = sharesHeld * finalPrice;
        std::cout << "End of data reached: Sold " << sharesHeld << " shares at " << finalPrice << std::endl;
    }

    std::cout << "Final Capital: " << initialCapital << std::endl;
    return initialCapital;
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
