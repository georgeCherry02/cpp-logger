#include <logger.h>
#include <logging_level.h>

#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <iostream>
#include <optional>
#include <regex>  // If you solve a problem with regex you now have two problems...
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

bool validate_single_message_written(const std::string& file_path,
                                     LoggingLevel&& level,
                                     const std::string& expected_message) {
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
    ss << expected_message << "$";
    std::regex r{ss.str()};
    return validate_log_file(file_path, {r});
};

std::regex construct_expected_log_line(const LoggingLevel& level,
                                       const std::string& message) {
    std::stringstream ss;
    ss << "*" << message;
    return std::regex{ss.str()};
};

}  // namespace utils

class FileHandlingFixture {
    const std::string d_file_path;

   public:
    FileHandlingFixture(const std::string& file_path) : d_file_path{file_path} {
        std::remove(d_file_path.c_str());
    };
    ~FileHandlingFixture() { std::remove(d_file_path.c_str()); }
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
    // FileHandlingFixture fh{"test.txt"};
    GIVEN("An operating `Logger` and a basic message") {
        const std::string file_path{"test.txt"};
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
}

}  // namespace test
}  // namespace logger
