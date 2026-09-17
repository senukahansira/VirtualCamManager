#ifndef VIDEOMANAGER_H
#define VIDEOMANAGER_H

#include <QObject>
#include <QVector>

#include "VideoItem.h"
#include "FFmpegProcess.h"

/*
 * VideoManager
 *
 * This class manages:
 *
 * - The list of videos
 * - Which video is currently playing
 * - Starting/stopping FFmpeg
 *
 * The GUI does not need to know how FFmpeg works.
 *
 * It simply tells VideoManager:
 *
 *     "Play this video."
 */
class VideoManager : public QObject
{
    Q_OBJECT

public:

    // Maximum number of videos allowed.
    static constexpr int MAX_VIDEOS = 10;

    explicit VideoManager(QObject *parent = nullptr);

    // Add a video to the list.
    bool addVideo(const QString &path);

    // Remove a video by index.
    void removeVideo(int index);

    // Return all videos.
    const QVector<VideoItem> &videos() const;

    // Return number of videos.
    int count() const;

    // Play a video at a specific index.
    bool play(int index);

    // Stop the currently playing video.
    void stop();

    // Check whether something is playing.
    bool isPlaying() const;

    // Return currently playing index.
    int currentIndex() const;

    // Set the V4L2 device that FFmpeg should use.
    void setCameraDevice(const QString &devicePath);

    // Return current camera device.
    QString cameraDevice() const;

signals:

    // Video list changed.
    void videosChanged();

    // A video started.
    void videoStarted(int index);

    // Playback stopped.
    void videoStopped();

    // Error.
    void errorOccurred(const QString &message);

    // FFmpeg output.
    void ffmpegOutput(const QString &output);

private:

    // All videos.
    QVector<VideoItem> m_videos;

    // FFmpeg controller.
    FFmpegProcess *m_ffmpeg;

    // Currently playing video index.
    int m_currentIndex;

    // Current V4L2 camera device.
    QString m_cameraDevice;
};

#endif // VIDEOMANAGER_H