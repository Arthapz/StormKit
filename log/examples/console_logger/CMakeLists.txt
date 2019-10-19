project(ConsoleLogger VERSION 0.1.0 LANGUAGES CXX)

add_executable(
    console_logger
    src/main.cpp
)

target_link_libraries(
    console_logger
    PUBLIC
        StormKit::core
        StormKit::log
)

enable_cpp17(console_logger "${STORM_COMPILER}")
copy_imported_targets(console_logger
    StormKit::core
    StormKit::log
)
