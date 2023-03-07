add_executable(LoggerIntegration logger_instantiation.t.cpp)

target_link_libraries(LoggerIntegration PRIVATE Logger Catch2::Catch2WithMain)

add_test(NAME LoggerIntegration COMMAND LoggerIntegration)
