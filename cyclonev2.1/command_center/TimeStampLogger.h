#ifndef TIMESTAMP_LOGGER_H
#define TIMESTAMP_LOGGER_H

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

class TimestampLogger {
public:

    static std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

        return oss.str();
    }


    static void writeToFile(const std::string& filename, const std::string& data) {
        std::ofstream file;
        file.open(filename, std::ios::app);

        if (!file) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return;
        }

        file << data << std::endl;
        file.close();
    }
};

#endif 