#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <iomanip> // For formatting numbers

class Logger {
private:
    std::ofstream file;
    std::mutex log_mutex;

    Logger() {
        file.open("output.txt", std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            throw std::ios_base::failure("Failed to open log file.");
        }
    }

    ~Logger() {
        if (file.is_open()) {
            file.close();
        }
    }

public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    Logger(const Logger&) = delete;
    void operator=(const Logger&) = delete;

    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(log_mutex);
        std::cout << message << std::endl;
        if (file.is_open()) {
            file << message << std::endl;
        }
    }

    void logOrder(int id, const char side, double price, int quantity,
                  const std::string& action, const std::string& matchedWith,
                  int latency) {
        std::lock_guard<std::mutex> lock(log_mutex);
        std::ostringstream oss;

        oss << "=========================================\n"
            << "Order ID:      " << id << "\n"
            << "Type:          " << side << "\n"
            << "Price:         " << std::fixed << std::setprecision(2) << price << "\n"
            << "Quantity:      " << quantity << "\n"
            << "Action:        " << action << "\n"
            << "Matched With:  " << matchedWith << "\n"
            << "Latency:       " << latency << " microseconds\n"
            << "-----------------------------------------";

        std::string formattedLog = oss.str();
        std::cout << formattedLog << std::endl;
        if (file.is_open()) {
            file << formattedLog << std::endl;
        }
    }
};

#endif // LOGGER_H
