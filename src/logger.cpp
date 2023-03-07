#include <logger.h>

#include <chrono>
#include <iostream>

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

const char* Logger::get_time() {
    time_t raw_time;
    struct tm* timeinfo;
    std::time(&raw_time);
    timeinfo = std::localtime(&raw_time);
    return std::asctime(timeinfo);
}

}  // namespace logger
