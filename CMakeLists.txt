cmake_minimum_required(VERSION 3.16)

project(VirtualCamManager
    VERSION 1.0
    LANGUAGES CXX
)

# Use modern C++.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt's automatic processing of:
#
# Q_OBJECT
# signals
# slots
#
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 Widgets.
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Source files.
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

    # NEW:
    # Detects VirtualCam automatically.
    src/VirtualCameraManager.cpp
    src/VirtualCameraManager.h
)

# Link Qt Widgets.
target_link_libraries(VirtualCamManager
    PRIVATE
        Qt6::Widgets
)