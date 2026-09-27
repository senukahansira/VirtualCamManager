#include "VideoManager.h"

#include <QFileInfo>

VideoManager::VideoManager(QObject *parent)
    : QObject(parent),
      m_currentVideoIndex(-1),
      m_loop(true)
{
    connect(
        &m_ffmpeg,
        &FFmpegProcess::positionChanged,
        this,
        &VideoManager::positionChanged
    );

    connect(
        &m_ffmpeg,
        &FFmpegProcess::durationChanged,
        this,
        [this](qint64 duration)
        {
            if (m_currentVideoIndex >= 0 &&
                m_currentVideoIndex < m_videos.size())
            {
                m_videos[m_currentVideoIndex]
                    ->setDurationMs(duration);
            }

            emit durationChanged(duration);
        }
    );

    connect(
        &m_ffmpeg,
        &FFmpegProcess::errorOccurred,
        this,
        &VideoManager::errorMessage
    );

    connect(
        &m_ffmpeg,
        &FFmpegProcess::started,
        this,
        [this]()
        {
            emit playingChanged(true);
            emit pausedChanged(false);
            emit statusMessage("Video is playing.");
        }
    );

    connect(
        &m_ffmpeg,
        &FFmpegProcess::stopped,
        this,
        [this]()
        {
            emit playingChanged(false);
        }
    );

    connect(
        &m_ffmpeg,
        &FFmpegProcess::paused,
        this,
        [this]()
        {
            emit playingChanged(false);
            emit pausedChanged(true);
            emit statusMessage("Video paused.");
        }
    );

    connect(
        &m_ffmpeg,
        &FFmpegProcess::resumed,
        this,
        [this]()
        {
            emit playingChanged(true);
            emit pausedChanged(false);
            emit statusMessage("Video resumed.");
        }
    );

    connect(
        &m_thumbnailGenerator,
        &ThumbnailGenerator::thumbnailReady,
        this,
        [this](
            const QString &path,
            const QPixmap &pixmap)
        {
            for (VideoItem *item : m_videos)
            {
                if (item->filePath() == path)
                {
                    item->setThumbnail(pixmap);
                    emit videosChanged();
                    break;
                }
            }
        }
    );

    connect(
        &m_thumbnailGenerator,
        &ThumbnailGenerator::error,
        this,
        [this](
            const QString &,
            const QString &message)
        {
            emit statusMessage(message);
        }
    );
}

bool VideoManager::addVideo(
    const QString &filePath)
{
    if (m_videos.size() >= MAX_VIDEOS)
    {
        emit errorMessage(
            "Maximum of 10 videos reached."
        );
        return false;
    }

    QFileInfo info(filePath);

    if (!info.exists() || !info.isFile())
    {
        emit errorMessage(
            "Video file does not exist."
        );
        return false;
    }

    for (VideoItem *item : m_videos)
    {
        if (item->filePath() == filePath)
        {
            emit errorMessage(
                "This video is already in the list."
            );
            return false;
        }
    }

    VideoItem *item =
        new VideoItem(filePath);

    m_videos.append(item);

    m_thumbnailGenerator.generate(
        item->filePath(),
        item->thumbnailPath()
    );

    emit videosChanged();

    emit statusMessage(
        QString("Added: %1")
            .arg(item->fileName())
    );

    return true;
}

bool VideoManager::removeVideo(int index)
{
    if (index < 0 || index >= m_videos.size())
        return false;

    if (index == m_currentVideoIndex)
    {
        stop();
        m_currentVideoIndex = -1;
    }

    delete m_videos[index];
    m_videos.removeAt(index);

    if (m_currentVideoIndex > index)
        --m_currentVideoIndex;

    emit videosChanged();

    return true;
}

int VideoManager::videoCount() const
{
    return m_videos.size();
}

VideoItem *VideoManager::videoAt(int index)
{
    if (index < 0 || index >= m_videos.size())
        return nullptr;

    return m_videos[index];
}

void VideoManager::playVideo(int index)
{
    VideoItem *item = videoAt(index);

    if (!item)
        return;

    if (m_ffmpeg.isRunning() ||
        m_ffmpeg.isPaused())
    {
        m_ffmpeg.stop();
    }

    m_currentVideoIndex = index;

    emit currentVideoChanged(index);

    m_ffmpeg.setLoop(m_loop);

    m_ffmpeg.start(item->filePath(), 0);

    emit statusMessage(
        QString("Playing: %1")
            .arg(item->fileName())
    );
}

void VideoManager::stop()
{
    m_ffmpeg.stop();

    emit playingChanged(false);
    emit pausedChanged(false);
    emit statusMessage("Playback stopped.");
}

void VideoManager::pause()
{
    m_ffmpeg.pause();
}

void VideoManager::resume()
{
    m_ffmpeg.resume();
}

void VideoManager::seek(qint64 positionMs)
{
    m_ffmpeg.seek(positionMs);
}

void VideoManager::setLoop(bool enabled)
{
    m_loop = enabled;
    m_ffmpeg.setLoop(enabled);
}

void VideoManager::setHardwareAcceleration(
    HardwareAcceleration::Method method)
{
    m_ffmpeg.setHardwareAcceleration(method);
}

bool VideoManager::isPlaying() const
{
    return m_ffmpeg.isRunning();
}

bool VideoManager::isPaused() const
{
    return m_ffmpeg.isPaused();
}

int VideoManager::currentVideoIndex() const
{
    return m_currentVideoIndex;
}
