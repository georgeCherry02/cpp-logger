#include <logger.h>

#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

namespace logger {
namespace test {

namespace utils {
bool validate_message_written(const std::string& file_path,
                              const std::string& expected_message) {
    std::ifstream fh{file_path};
    if (!fh.is_open()) {
        return false;
    }
    std::string output;
    std::cout << "Reading in file:" << std::endl;
    while (std::getline(fh, output)) {
        std::cout << output << std::endl;
        return expected_message == output;
    }
    return false;
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
            logger->output(message);
            THEN("The file being written to contains the message") {
                CHECK(utils::validate_message_written(file_path, message));
            }
        }
    }
}

}  // namespace test
}  // namespace logger
