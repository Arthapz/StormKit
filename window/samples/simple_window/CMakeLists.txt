project(SimpleWindow VERSION 0.1.0 LANGUAGES CXX)

add_executable(
        simple_window
	src/main.cpp
)

target_link_libraries(
    simple_window
    PUBLIC
        StormKit::core
        StormKit::window
        StormKit::log
)

enable_cpp17(simple_window "${STORM_COMPILER}")
copy_imported_targets(simple_window
	StormKit::window
	StormKit::log
)

if(${STORM_BUILD_TYPE} EQUAL 1)
    enable_sanitizers(simple_window)
endif()
