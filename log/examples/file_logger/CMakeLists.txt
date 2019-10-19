project(FileLogger VERSION 0.1.0 LANGUAGES C CXX)

add_executable(
    file_logger
    src/main.cpp
)

target_link_libraries(
    file_logger
    PUBLIC
        StormKit::core
        StormKit::log
)

enable_cpp17(file_logger "${STORM_COMPILER}")
copy_imported_targets(file_logger 
	StormKit::window
	StormKit::log
)
