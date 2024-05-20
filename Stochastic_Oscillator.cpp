#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>  

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
    std::string line;

    if (file.is_open()) {
        std::getline(file, line); 

        while (std::getline(file, line)) {
            std::istringstream s(line);
            MarketData dayData;
            std::getline(s, dayData.date, ',');
            std::getline(s, line, ',');
            dayData.open = std::stod(line);
            std::getline(s, line, ',');
            dayData.high = std::stod(line);
            std::getline(s, line, ',');
            dayData.low = std::stod(line);
            std::getline(s, line, ',');
            dayData.close = std::stod(line);
            
            data.push_back(dayData);
        }
        file.close();
    } else {
        std::cerr << "Could not open the file." << std::endl;
    }
    return data;
}

void calculateStochasticOscillator(const std::vector<MarketData>& data, int period = 14) {
    std::vector<double> _K_values;
    std::vector<double> _D_values;

    for (size_t i = period - 1; i < data.size(); ++i) {
        double lowestLow = std::numeric_limits<double>::max();
        double highestHigh = std::numeric_limits<double>::lowest();

        for (size_t j = i - period + 1; j <= i; ++j) {
            if (data[j].low < lowestLow) lowestLow = data[j].low;
            if (data[j].high > highestHigh) highestHigh = data[j].high;
        }

        double close = data[i].close;
        double _K = ((close - lowestLow) / (highestHigh - lowestLow)) * 100;
        _K_values.push_back(_K);

        if (_K_values.size() >= period) {
            double sum = 0;
            for (size_t k = _K_values.size() - period; k < _K_values.size(); ++k) {
                sum += _K_values[k];
            }
            double _D = sum / period;
            _D_values.push_back(_D);

            std::cout << "Date: " << data[i].date
                      << " _K: " << _K
                      << " _D: " << _D << std::endl;
        }
    }
}

int main() {
    std::string dataFile = "NASDAQ50.csv"; 
    std::vector<MarketData> data = readData(dataFile);
    if (!data.empty()) {
        calculateStochasticOscillator(data);
    } else {
        std::cerr << "No data available." << std::endl;
    }
    return 0;
}
