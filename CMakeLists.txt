cmake_minimum_required(VERSION 3.16)

# Name of our application
project(VirtualCamManager VERSION 1.0 LANGUAGES CXX)

# We use modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt's automatic processing of things like signals and slots.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt 6.
# Widgets gives us normal desktop GUI components such as:
# buttons, labels, layouts, windows, etc.
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# All source files in our application.
add_executable(VirtualCamManager

    src/main.cpp

    src/MainWindow.cpp
    src/MainWindow.h

    src/VideoManager.cpp
    src/VideoManager.h

    src/VideoItem.cpp
    src/VideoItem.h

    src/FFmpegProcess.cpp
    src/FFmpegProcess.h
)

# Link our program with Qt Widgets.
target_link_libraries(VirtualCamManager
    PRIVATE
    Qt6::Widgets
)