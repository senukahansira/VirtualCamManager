#ifndef VIDEOMANAGER_H
#define VIDEOMANAGER_H

#include <QObject>
#include <QVector>

#include "VideoItem.h"
#include "FFmpegProcess.h"
#include "ThumbnailGenerator.h"

class VideoManager : public QObject
{
    Q_OBJECT

public:
    static constexpr int MAX_VIDEOS = 10;

    explicit VideoManager(QObject *parent = nullptr);

    bool addVideo(const QString &filePath);
    bool removeVideo(int index);

    int videoCount() const;
    VideoItem *videoAt(int index);

    void playVideo(int index);
    void stop();
    void pause();
    void resume();
    void seek(qint64 positionMs);

    void setLoop(bool enabled);

    void setHardwareAcceleration(
        HardwareAcceleration::Method method
    );

    bool isPlaying() const;
    bool isPaused() const;

    int currentVideoIndex() const;

signals:
    void videosChanged();
    void currentVideoChanged(int index);

    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);

    void playingChanged(bool playing);
    void pausedChanged(bool paused);

    void statusMessage(const QString &message);
    void errorMessage(const QString &message);

private:
    QVector<VideoItem*> m_videos;

    FFmpegProcess m_ffmpeg;
    ThumbnailGenerator m_thumbnailGenerator;

    int m_currentVideoIndex;
    bool m_loop;
};

#endif
