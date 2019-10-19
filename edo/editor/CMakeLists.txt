project(EDITOR VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Qml Quick Widgets REQUIRED)

add_executable(editor
    src/main.cpp
)

target_compile_definitions(
    editor
    PRIVATE
        QT_DEPRECATED_WARNINGS
)
target_link_libraries(editor Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick Qt5::Widgets)
