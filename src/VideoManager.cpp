#include "VideoManager.h"

#include <QFileInfo>

/*
 * Constructor.
 */
VideoManager::VideoManager(QObject *parent)
    : QObject(parent),
      m_currentVideoIndex(-1)
{
    /*
     * When FFmpeg starts, tell the GUI.
     */
    connect(&m_ffmpeg,
            &FFmpegProcess::started,
            this,
            [this]()
            {
                emit statusMessage("Video is now playing.");
            });

    /*
     * When FFmpeg stops.
     */
    connect(&m_ffmpeg,
            &FFmpegProcess::stopped,
            this,
            [this]()
            {
                /*
                 * Only reset the current index if FFmpeg
                 * isn't immediately being restarted.
                 */
                emit statusMessage("Video stopped.");
            });

    /*
     * Forward FFmpeg error messages.
     */
    connect(&m_ffmpeg,
            &FFmpegProcess::errorOccurred,
            this,
            [this](const QString &message)
            {
                emit errorMessage(message);
            });
}


/*
 * Add a video to our list.
 */
bool VideoManager::addVideo(const QString &filePath)
{
    /*
     * Don't allow more than 10 videos.
     */
    if (m_videos.size() >= MAX_VIDEOS)
    {
        emit errorMessage(
            "You can only add a maximum of 10 videos."
        );

        return false;
    }

    /*
     * Check whether the file actually exists.
     */
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists() || !fileInfo.isFile())
    {
        emit errorMessage(
            "The selected video file does not exist."
        );

        return false;
    }

    /*
     * Prevent adding exactly the same file twice.
     */
    for (VideoItem *video : m_videos)
    {
        if (video->filePath() == filePath)
        {
            emit errorMessage(
                "This video has already been added."
            );

            return false;
        }
    }

    /*
     * Create a new VideoItem.
     *
     * We use the full file path.
     */
    VideoItem *video = new VideoItem(filePath);

    m_videos.append(video);

    emit videosChanged();

    emit statusMessage(
        QString("Added video: %1").arg(video->fileName())
    );

    return true;
}


/*
 * Remove a video.
 */
bool VideoManager::removeVideo(int index)
{
    /*
     * Make sure the index is valid.
     */
    if (index < 0 || index >= m_videos.size())
    {
        return false;
    }

    /*
     * If the video being removed is currently playing,
     * stop FFmpeg first.
     */
    if (index == m_currentVideoIndex)
    {
        stopVideo();
    }

    /*
     * Delete the VideoItem object.
     */
    delete m_videos[index];

    /*
     * Remove it from the QVector.
     */
    m_videos.removeAt(index);

    /*
     * If our current video index was after the removed item,
     * move it back by one.
     */
    if (m_currentVideoIndex > index)
    {
        --m_currentVideoIndex;
    }

    emit videosChanged();

    return true;
}


/*
 * Return the number of videos.
 */
int VideoManager::videoCount() const
{
    return m_videos.size();
}


/*
 * Return a VideoItem at a particular index.
 */
VideoItem *VideoManager::videoAt(int index)
{
    if (index < 0 || index >= m_videos.size())
    {
        return nullptr;
    }

    return m_videos[index];
}


/*
 * Play a video.
 */
void VideoManager::playVideo(int index)
{
    /*
     * Check the index.
     */
    if (index < 0 || index >= m_videos.size())
    {
        return;
    }

    VideoItem *video = m_videos[index];

    if (!video)
    {
        return;
    }

    /*
     * If another video is playing,
     * stop it first.
     */
    if (m_ffmpeg.isRunning())
    {
        m_ffmpeg.stop();
    }

    /*
     * Remember which video is active.
     */
    m_currentVideoIndex = index;

    emit currentVideoChanged(index);

    emit statusMessage(
        QString("Starting: %1").arg(video->fileName())
    );

    /*
     * Start FFmpeg with the selected file.
     */
    m_ffmpeg.start(video->filePath());
}


/*
 * Stop the currently playing video.
 */
void VideoManager::stopVideo()
{
    if (m_ffmpeg.isRunning())
    {
        m_ffmpeg.stop();
    }

    m_currentVideoIndex = -1;

    emit currentVideoChanged(-1);

    emit statusMessage("No video is playing.");
}


/*
 * Return the current video index.
 */
int VideoManager::currentVideoIndex() const
{
    return m_currentVideoIndex;
}


/*
 * Return whether FFmpeg is running.
 */
bool VideoManager::isPlaying() const
{
    return m_ffmpeg.isRunning();
}