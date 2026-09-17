#ifndef VIDEOMANAGER_H
#define VIDEOMANAGER_H

#include <QObject>
#include <QVector>

#include "VideoItem.h"
#include "FFmpegProcess.h"

/*
 * VideoManager
 *
 * Responsibilities:
 *
 * - Store videos
 * - Maximum 10 videos
 * - Add videos
 * - Remove videos
 * - Play a video
 * - Stop the current video
 */
class VideoManager : public QObject
{
    Q_OBJECT

public:

    explicit VideoManager(QObject *parent = nullptr);

    // Maximum number of videos.
    static constexpr int MAX_VIDEOS = 10;

    // Add a video.
    bool addVideo(const QString &filePath);

    // Remove video at a specific index.
    bool removeVideo(int index);

    // Get number of videos.
    int videoCount() const;

    // Get video at index.
    VideoItem *videoAt(int index);

    // Start playing a specific video.
    void playVideo(int index);

    // Stop current video.
    void stopVideo();

    // Return index of currently playing video.
    int currentVideoIndex() const;

    // Return true if FFmpeg is running.
    bool isPlaying() const;

signals:

    // Something changed in our video list.
    void videosChanged();

    // Current video changed.
    void currentVideoChanged(int index);

    // FFmpeg status message.
    void statusMessage(const QString &message);

    // Error message.
    void errorMessage(const QString &message);

private:

    // List of our videos.
    QVector<VideoItem*> m_videos;

    // Controls FFmpeg.
    FFmpegProcess m_ffmpeg;

    // Index of currently playing video.
    // -1 means no video is playing.
    int m_currentVideoIndex;

    Q_DISABLE_COPY(VideoManager)
};

#endif // VIDEOMANAGER_H