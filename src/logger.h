#pragma once

#include <logging_level.h>

#include <fstream>
#include <memory>
#include <optional>
#include <string>

namespace logger {

class Logger {
   private:
    LoggingLevel d_level;
    std::ofstream d_file;

    Logger(LoggingLevel&& level, std::ofstream&& fh)
        : d_level{std::move(level)}, d_file{std::move(fh)} {};

    void decorate(LoggingLevel&& level);

   public:
    static std::optional<Logger> get_logger(LoggingLevel level,
                                            const std::string& output_path);
    void set_level(LoggingLevel level);

    void output(LoggingLevel level, const std::string& output_line) {
        decorate(std::move(level));
        d_file << output_line << std::endl;
        return;
    }
};

}  // namespace logger
