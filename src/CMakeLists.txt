add_library(Logger logger.cpp logging_level.cpp)

find_package(date REQUIRED)

target_include_directories(Logger PUBLIC .)
target_link_libraries(Logger PUBLIC date::date)
