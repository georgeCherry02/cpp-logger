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

    const char* get_time();

   public:
    static std::optional<Logger> get_logger(LoggingLevel&& level,
                                            const std::string& output_path);

    void set_level(LoggingLevel&& level);

    template <typename... ARGS>
    void output(const std::string& output_line, ARGS&&...) {
        d_file << get_time() << " " << output_line << std::endl;
        return;
    }

    template <typename... ARGS>
    void output(std::string&& output_line, ARGS&&...) {
        d_file << get_time() << " " << output_line << std::endl;
        return;
    }
};

}  // namespace logger
