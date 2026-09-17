# Virtual Camera Manager
<img width="1166" height="681" alt="2026-09-17_15-25" src="https://github.com/user-attachments/assets/276ef1ce-0ba9-46f4-a376-c5cabdc93c85" />

A simple Linux virtual camera manager written in C++ and Qt 6.

The application uses:

- C++
- Qt 6
- FFmpeg
- v4l2loopback

The virtual camera output is:

    /dev/video10

---

# Features

Current version:

- Add videos
- Maximum 10 videos
- Remove videos
- Play video
- Stop video
- Loop video forever
- FFmpeg output to v4l2loopback
- 1280x720 output
- YUV420P pixel format

Only one video can be output to the virtual camera at a time.

---

# Requirements

Ubuntu/Debian:

    sudo apt update

    sudo apt install build-essential cmake qt6-base-dev ffmpeg

Install v4l2loopback:

    sudo apt install v4l2loopback-dkms v4l2loopback-utils

---

# Start the virtual camera

First load v4l2loopback:

    sudo modprobe v4l2loopback \
        devices=1 \
        video_nr=10 \
        card_label="VirtualCam" \
        exclusive_caps=1

Check the device:

    ls -l /dev/video10

You can also use:

    v4l2-ctl --list-devices

---

# Build

From the VirtualCamManager directory:

    mkdir build

    cd build

    cmake ..

    make -j$(nproc)

---

# Run

From the build directory:

    ./VirtualCamManager

---

# Test FFmpeg manually

Before using the GUI, you can test:

    ffmpeg -nostdin \
        -re \
        -stream_loop -1 \
        -i ll.mp4 \
        -vf scale=1280:720,format=yuv420p \
        -pix_fmt yuv420p \
        -f v4l2 \
        /dev/video10

If this works, the GUI should be able to use the same FFmpeg pipeline.

---

# How the program works

The application contains four main parts.

## MainWindow

Creates the GUI.

It contains:

- Add Video button
- Video list
- Play buttons
- Stop buttons
- Remove buttons
- Status display

---

## VideoItem

Represents one video.

It stores:

- Video file path
- Video filename

---

## VideoManager

Manages the video list.

It:

- Stores up to 10 videos
- Adds videos
- Removes videos
- Starts videos
- Stops videos

---

## FFmpegProcess

Starts and stops FFmpeg.

The application effectively runs:

    ffmpeg -nostdin -re -stream_loop -1 \
        -i VIDEO_FILE \
        -vf scale=1280:720,format=yuv420p \
        -pix_fmt yuv420p \
        -f v4l2 \
        /dev/video10

---

# Important

The program does not currently load the v4l2loopback kernel module automatically.

Run:

    sudo modprobe v4l2loopback \
        devices=1 \
        video_nr=10 \
        card_label="VirtualCam" \
        exclusive_caps=1

before starting the application.

---

