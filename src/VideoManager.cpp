#include "VideoManager.h"

/*
 * Constructor
 */
VideoManager::VideoManager(QObject *parent)
    : QObject(parent),
      m_ffmpeg(new FFmpegProcess(this)),
      m_currentIndex(-1)
{
    /*
     * Forward FFmpeg signals to whoever is using VideoManager.
     *
     * This keeps FFmpeg details hidden from the GUI.
     */
    connect(m_ffmpeg,
            &FFmpegProcess::errorOccurred,
            this,
            &VideoManager::errorOccurred);

    connect(m_ffmpeg,
            &FFmpegProcess::outputReceived,
            this,
            &VideoManager::ffmpegOutput);

    connect(m_ffmpeg,
            &FFmpegProcess::stopped,
            this,
            [this]()
    {
        m_currentIndex = -1;
        emit videoStopped();
    });
}


/*
 * Add a video.
 */
bool VideoManager::addVideo(const QString &path)
{
    // Do not allow more than 10 videos.
    if (m_videos.size() >= MAX_VIDEOS)
        return false;

    // Do not add an empty path.
    if (path.isEmpty())
        return false;

    // Prevent duplicate files.
    for (const VideoItem &video : m_videos)
    {
        if (video.path() == path)
            return false;
    }

    // Add the new video.
    m_videos.append(VideoItem(path));

    emit videosChanged();

    return true;
}


/*
 * Remove a video.
 */
void VideoManager::removeVideo(int index)
{
    // Check that index is valid.
    if (index < 0 || index >= m_videos.size())
        return;

    /*
     * If the video being removed is currently playing,
     * stop FFmpeg first.
     */
    if (index == m_currentIndex)
    {
        stop();
    }

    m_videos.removeAt(index);

    /*
     * If a video before the current one was removed,
     * adjust the index.
     */
    if (m_currentIndex > index)
    {
        --m_currentIndex;
    }

    emit videosChanged();
}


/*
 * Return the video list.
 */
const QVector<VideoItem> &VideoManager::videos() const
{
    return m_videos;
}


/*
 * Return number of videos.
 */
int VideoManager::count() const
{
    return m_videos.size();
}


/*
 * Start a video.
 */
bool VideoManager::play(int index)
{
    /*
     * Validate the index.
     */
    if (index < 0 || index >= m_videos.size())
    {
        emit errorOccurred("Invalid video selected.");
        return false;
    }

    /*
     * We need a virtual camera before starting FFmpeg.
     */
    if (m_cameraDevice.isEmpty())
    {
        emit errorOccurred(
            "No VirtualCam device has been detected."
        );

        return false;
    }

    /*
     * Start FFmpeg with the selected video.
     *
     * If another video is playing, FFmpegProcess::start()
     * automatically stops it first.
     */
    m_ffmpeg->start(
        m_videos[index].path(),
        m_cameraDevice
    );

    m_currentIndex = index;

    emit videoStarted(index);

    return true;
}


/*
 * Stop playback.
 */
void VideoManager::stop()
{
    if (!m_ffmpeg->isRunning())
    {
        m_currentIndex = -1;
        return;
    }

    m_ffmpeg->stop();

    m_currentIndex = -1;

    emit videoStopped();
}


/*
 * Check whether FFmpeg is running.
 */
bool VideoManager::isPlaying() const
{
    return m_ffmpeg->isRunning();
}


/*
 * Return current video index.
 */
int VideoManager::currentIndex() const
{
    return m_currentIndex;
}


/*
 * Set the V4L2 device.
 */
void VideoManager::setCameraDevice(const QString &devicePath)
{
    m_cameraDevice = devicePath;
}


/*
 * Return the V4L2 device.
 */
QString VideoManager::cameraDevice() const
{
    return m_cameraDevice;
}