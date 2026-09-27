cmake_minimum_required(VERSION 3.16)

project(VirtualCamManager VERSION 2.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(VirtualCamManager
    src/main.cpp

    src/MainWindow.cpp
    src/MainWindow.h
    src/ClickableSlider.h

    src/VideoItem.cpp
    src/VideoItem.h

    src/VideoManager.cpp
    src/VideoManager.h

    src/FFmpegProcess.cpp
    src/FFmpegProcess.h

    src/ThumbnailGenerator.cpp
    src/ThumbnailGenerator.h

    src/HardwareAcceleration.cpp
    src/HardwareAcceleration.h

    resources/resources.qrc
)
target_link_libraries(VirtualCamManager PRIVATE Qt6::Widgets)

install(TARGETS VirtualCamManager RUNTIME DESTINATION bin)
