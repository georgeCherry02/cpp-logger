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

void Logger::decorate() {
    using namespace std::chrono;
    auto time = floor<milliseconds>(system_clock::now());
    using namespace date;
    d_file << time << " ";
}

}  // namespace logger
