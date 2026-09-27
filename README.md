# Virtual Camera Manager V2.1.0

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B" alt="C++" />
  <img src="https://img.shields.io/badge/Qt-6-41CD52?style=for-the-badge&logo=qt" alt="Qt 6" />
  <img src="https://img.shields.io/badge/Linux-V4L2-FCC624?style=for-the-badge&logo=linux&logoColor=black" alt="Linux V4L2" />
  <img src="https://img.shields.io/badge/FFmpeg-Media%20Pipeline-007808?style=for-the-badge&logo=ffmpeg" alt="FFmpeg" />
</p>

A simple Linux desktop application for managing videos and streaming them to a virtual camera created with **v4l2loopback**.

The application provides a Qt 6 graphical interface for adding videos, selecting a video to play, and sending the selected video to `/dev/video10` through FFmpeg.

## ✨ What it does

- Adds videos to a managed list
- Supports up to 10 videos
- Plays a selected video through the virtual camera
- Stops the currently playing video
- Removes videos from the list
- Loops video playback continuously
- Converts video output to 1280x720 YUV420P
- Uses FFmpeg to stream video to v4l2loopback
- Allows only one video to be active at a time

## 🧩 Tech stack

- C++
- Qt 6
- CMake
- FFmpeg
- v4l2loopback
- Video4Linux2 (V4L2)

## 🏗️ How it works

1. The v4l2loopback kernel module creates a virtual camera device.
2. The application loads video files into the video list.
3. The user selects a video and presses **Play**.
4. The application starts an FFmpeg process for the selected file.
5. FFmpeg scales the video to `1280x720`, converts it to `yuv420p`, and writes it to `/dev/video10`.
6. Applications such as video-conferencing tools can use the virtual camera as a regular camera source.

## 📁 Project structure

```text
.
├── CMakeLists.txt       # CMake build configuration
├── src/                 # C++ application source files
├── include/             # Application headers
├── README.md            # Project documentation
└── build/               # Generated build directory
```

## 🛠️ Requirements

The application currently targets Ubuntu/Debian-based Linux distributions, but it also works on Arch-based systems with the required packages installed.

### Debian/Ubuntu

Install the required packages:

```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev ffmpeg
```

Install v4l2loopback:

```bash
sudo apt install v4l2loopback-dkms v4l2loopback-utils
```

### Arch Linux / Manjaro

Install the required packages:

```bash
sudo pacman -Syu
sudo pacman -S base-devel cmake qt6-base ffmpeg linux-headers
```

Install v4l2loopback:

```bash
sudo pacman -S v4l2loopback-dkms
```

If the `v4l2loopback-dkms` package is unavailable in your mirror, try the non-DKMS package:

```bash
sudo pacman -S v4l2loopback
```

## 📷 Create the virtual camera

Before starting the application, load the v4l2loopback kernel module:

```bash
sudo modprobe v4l2loopback \
    devices=1 \
    video_nr=10 \
    card_label="VirtualCam" \
    exclusive_caps=1
```

Check that the device was created:

```bash
ls -l /dev/video10
v4l2-ctl --list-devices
```

> The application does not currently load the v4l2loopback kernel module automatically. The module must be loaded before running the application.

### Make the module load automatically on Arch

```bash
echo "v4l2loopback" | sudo tee /etc/modules-load.d/v4l2loopback.conf
```

This will load the module automatically at boot.

## 🔨 Build

From the repository directory:

```bash
mkdir -p build
cd build
cmake ..
make -j$(nproc)
```

### Build on Arch

The build process is the same on Arch:

```bash
git clone https://github.com/senukahansira/VirtualCamManager.git
cd VirtualCamManager
mkdir -p build
cd build
cmake ..
make -j$(nproc)
```

## ▶️ Run

From the build directory:

```bash
./VirtualCamManager
```

Or from the repository root with the executable built in `build`:

```bash
./build/VirtualCamManager
```

### Run on Arch Linux

After installing dependencies and loading the module:

```bash
sudo modprobe v4l2loopback devices=1 video_nr=10 card_label="VirtualCam" exclusive_caps=1
./build/VirtualCamManager
```

Then add a video in the GUI and press **Play** to send it to `/dev/video10`.

## 🧪 Test FFmpeg manually

You can test the virtual camera and FFmpeg pipeline independently of the GUI:

```bash
ffmpeg \
    -nostdin \
    -re \
    -stream_loop -1 \
    -i video.mp4 \
    -vf scale=1280:720,format=yuv420p \
    -pix_fmt yuv420p \
    -f v4l2 \
    /dev/video10
```

Replace `video.mp4` with the path to a video file. If this command successfully writes to `/dev/video10`, the GUI should be able to use the same pipeline.

## 🧱 Main components

### MainWindow

Provides the graphical user interface, including:

- Add Video button
- Video list
- Play controls
- Stop controls
- Remove controls
- Status display

### VideoItem

Represents a video in the application and stores its file path and filename.

### VideoManager

Manages the video collection and controls video playback. It stores up to 10 videos and ensures that only one video is sent to the virtual camera at a time.

### FFmpegProcess

Starts and stops FFmpeg using a pipeline equivalent to:

```bash
ffmpeg -nostdin -re -stream_loop -1 \
    -i VIDEO_FILE \
    -vf scale=1280:720,format=yuv420p \
    -pix_fmt yuv420p \
    -f v4l2 \
    /dev/video10
```

## Hardware acceleration

The GUI offers:

- Auto
- Software
- VAAPI
- CUDA
- QSV
- VDPAU

Auto checks:

```bash
ffmpeg -hide_banner -hwaccels
```

Hardware API availability does not guarantee that every codec/GPU/driver combination will work. If hardware decoding fails, choose Software.

## Thumbnail cache

Thumbnails are stored in:

```text
~/.cache/VirtualCamManager/
```

## Output

```text
1280x720
YUV420P
V4L2
/dev/video10
```

## Note about pause/seek

This V2 implementation pauses by remembering the current timestamp and stopping FFmpeg. Resume starts FFmpeg from the saved timestamp.

Seeking similarly restarts FFmpeg from the requested timestamp. This is simple and reliable for the initial version, but V2.1 can replace this with a persistent media pipeline for smoother seeking.

## Version

Virtual Camera Manager 2.1.0

## 🖼️ Screenshot

<img width="1919" height="1079" alt="2026-09-27_06-05" src="https://github.com/user-attachments/assets/6a8e4c20-e9c8-458b-b161-6c7de0ed648d" />

## ⚠️ Notes

- The default virtual camera device is `/dev/video10`.
- Only one video can be output to the virtual camera at a time.
- The v4l2loopback module must be loaded before launching the application.
- FFmpeg must be installed and available in the system `PATH`.
- On Arch, install `linux-headers` and `v4l2loopback-dkms` if you see module or build issues.

## Author

Built by [senuka hansira](https://github.com/senukahansira).
