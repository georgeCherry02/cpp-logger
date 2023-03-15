#include <logger.h>
#include <logging_level.h>

#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <iostream>
#include <optional>
#include <regex>  // If you solve a problem with regex you now have two problems...
#include <set>
#include <sstream>
#include <string>

namespace logger {
namespace test {

namespace utils {

bool validate_log_file(const std::string& file_path,
                       std::vector<std::regex>&& expected_logs) {
    std::ifstream fh{file_path};
    if (!fh.is_open()) {
        std::cout << "Failed to read in file when testing, path provided="
                  << file_path << std::endl;
        return false;
    }

    std::string output;
    bool still_matches{true};

    std::cout << "Reading in a file:" << std::endl;
    int target_line = 0;
    while (std::getline(fh, output)) {
        std::cout << output << std::endl;

        if (target_line > expected_logs.size()) {
            std::cout << "Expected more lines than were present in the file"
                      << std::endl;
            return false;
        }

        bool matches{std::regex_match(output, expected_logs.at(target_line))};
        if (!matches) {
            std::cout << "Line mismatch! In file: " << output << std::endl;
        }

        if (still_matches && !matches) {
            still_matches = false;
        }

        ++target_line;
    }
    return still_matches;
};

std::regex construct_expected_log_line(LoggingLevel level,
                                       const std::string& message) {
    std::stringstream ss;
    ss << "^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}.\\d{3} ";
    switch (level) {
        case LoggingLevel::ERROR:
            ss << "ERROR: ";
            break;
        case LoggingLevel::WARN:
            ss << "WARN: ";
            break;
        case LoggingLevel::INFO:
            ss << "INFO: ";
            break;
        case LoggingLevel::DEBUG:
            ss << "DEBUG: ";
            break;
    }
    ss << message << "$";
    return std::regex{ss.str()};
};

bool validate_single_message_written(const std::string& file_path,
                                     LoggingLevel level,
                                     const std::string& expected_message) {
    return validate_log_file(
        file_path, {construct_expected_log_line(level, expected_message)});
};

bool validate_messages_written(
    const std::string& file_path,
    const std::vector<std::pair<LoggingLevel, std::string>> messages) {
    std::vector<std::regex> expected_messages;
    expected_messages.reserve(messages.size());
    std::transform(messages.begin(), messages.end(),
                   std::back_inserter(expected_messages),
                   [](const auto& message) -> std::regex {
                       auto& [level, line] = message;
                       return construct_expected_log_line(level, line);
                   });
    return validate_log_file(file_path, std::move(expected_messages));
};

}  // namespace utils

class FileHandlingFixture {
    const std::string d_file_path;

   public:
    FileHandlingFixture(const std::string& file_path) : d_file_path{file_path} {
        std::remove(d_file_path.c_str());
    };
    ~FileHandlingFixture() { std::remove(d_file_path.c_str()); }

    void flush() { std::remove(d_file_path.c_str()); }
};

SCENARIO("Standard Initialisation") {
    FileHandlingFixture fh{"test.txt"};
    GIVEN("A valid file path and a logging level") {
        const std::string file_path{"test.txt"};
        WHEN("`get_logger` is called") {
            std::optional<Logger> logger{
                Logger::get_logger(LoggingLevel::INFO, file_path)};
            THEN(
                "A `Logger` is succesfully instantiated and there's a "
                "filehandle present") {
                REQUIRE(logger);
            }
        }
    }
}

SCENARIO("Outputting Data") {
    const std::string file_path{"test.txt"};
    FileHandlingFixture fh{file_path};
    GIVEN("An operating `Logger` and a basic message") {
        auto logger = Logger::get_logger(LoggingLevel::INFO, file_path);
        REQUIRE(logger);
        std::string message{"Basic message!"};
        WHEN("`output` is called on said `Logger`") {
            logger->output(LoggingLevel::INFO, message);
            THEN("The file being written to contains the message") {
                CHECK(utils::validate_single_message_written(
                    file_path, LoggingLevel::INFO, message));
            }
        }
    }

    fh.flush();
    GIVEN(
        "An operating `Logger`, a basic message and a variety of "
        "`LoggingLevel`s") {
        auto logger = Logger::get_logger(LoggingLevel::DEBUG, file_path);
        REQUIRE(logger);
        std::string message{"Basic message!"};
        std::set<LoggingLevel> levels{LoggingLevel::DEBUG, LoggingLevel::INFO,
                                      LoggingLevel::WARN, LoggingLevel::ERROR};
        WHEN("`output` is called on the `Logger`") {
            std::for_each(levels.begin(), levels.end(),
                          [&logger, &message](LoggingLevel level) {
                              logger->output(level, message);
                          });
            THEN("The file contains all messages at expected levels") {
                std::vector<std::pair<LoggingLevel, std::string>>
                    expected_messages;
                expected_messages.reserve(levels.size());
                std::transform(levels.begin(), levels.end(),
                               std::back_inserter(expected_messages),
                               [&message](LoggingLevel level) {
                                   return std::make_pair(level, message);
                               });

                CHECK(utils::validate_messages_written(file_path,
                                                       expected_messages));
            }
        }
    }
}

}  // namespace test
}  // namespace logger
