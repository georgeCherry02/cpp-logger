#include <date/date.h>
#include <logger.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>

namespace logger {

std::optional<Logger> Logger::get_logger(LoggingLevel&& level,
                                         const std::string& output_path) {
    std::ofstream fh{output_path};
    if (!fh.is_open()) {
        std::cerr << "Failed to open logging file=" << output_path;
        return std::nullopt;
    }
    return Logger{std::move(level), std::move(fh)};
};

void Logger::set_level(LoggingLevel&& level) { d_level = level; }

void Logger::decorate(LoggingLevel&& level) {
    using namespace std::chrono;
    auto time = floor<milliseconds>(system_clock::now());
    using namespace date;
    d_file << time << " ";
    switch (level) {
        case LoggingLevel::ERROR:
            d_file << "ERROR: ";
            break;
        case LoggingLevel::WARN:
            d_file << "WARN: ";
            break;
        case LoggingLevel::INFO:
            d_file << "INFO: ";
            break;
        case LoggingLevel::DEBUG:
            d_file << "DEBUG: ";
            break;
        default:
            d_file << "LOGGING ERROR (Invalid Level): ";
    }
}

}  // namespace logger
