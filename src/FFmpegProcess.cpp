#include "FFmpegProcess.h"

#include <QProcessEnvironment>

/*
 * Constructor
 */
FFmpegProcess::FFmpegProcess(QObject *parent)
    : QObject(parent),
      m_outputDevice("/dev/video0")
{
    /*
     * FFmpeg normally writes its logs to stderr.
     *
     * We capture that output so our application can
     * display useful information/debug messages.
     */
    connect(&m_process,
            &QProcess::readyReadStandardError,
            this,
            [this]()
            {
                QByteArray data = m_process.readAllStandardError();

                QString message = QString::fromLocal8Bit(data);

                emit outputMessage(message);
            });

    /*
     * This signal is emitted when QProcess has successfully
     * started FFmpeg.
     */
    connect(&m_process,
            &QProcess::started,
            this,
            [this]()
            {
                emit started();
            });

    /*
     * This signal is emitted when FFmpeg finishes.
     */
    connect(&m_process,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this](int exitCode, QProcess::ExitStatus exitStatus)
            {
                Q_UNUSED(exitCode);
                Q_UNUSED(exitStatus);

                emit stopped();
            });

    /*
     * If QProcess itself cannot start FFmpeg,
     * this signal will be emitted.
     */
    connect(&m_process,
            &QProcess::errorOccurred,
            this,
            [this](QProcess::ProcessError error)
            {
                QString message;

                switch (error)
                {
                case QProcess::FailedToStart:
                    message = "FFmpeg could not be started. "
                              "Is FFmpeg installed?";
                    break;

                case QProcess::Crashed:
                    message = "FFmpeg crashed.";
                    break;

                case QProcess::Timedout:
                    message = "FFmpeg operation timed out.";
                    break;

                default:
                    message = "Unknown FFmpeg process error.";
                    break;
                }

                emit errorOccurred(message);
            });
}


/*
 * Start FFmpeg.
 *
 * This is equivalent to running something like:
 *
 * ffmpeg -nostdin -re -stream_loop -1
 *        -i "video.mp4"
 *        -vf scale=1280:720,format=yuv420p
 *        -pix_fmt yuv420p
 *        -f v4l2 /dev/video0
 */
void FFmpegProcess::start(const QString &videoPath)
{
    /*
     * If another FFmpeg process is already running,
     * stop it first.
     */
    if (isRunning())
    {
        stop();
    }

    /*
     * Create the command-line arguments.
     *
     * IMPORTANT:
     *
     * We don't create one giant shell command.
     *
     * Each argument is passed separately to QProcess.
     * This is safer and also handles filenames containing spaces.
     */
    QStringList arguments;

    // Don't read commands from stdin.
    arguments << "-nostdin";

    // Read the video approximately in real time.
    arguments << "-re";

    // Loop the video forever.
    arguments << "-stream_loop" << "-1";

    // Input video.
    arguments << "-i" << videoPath;

    /*
     * Resize video to 1280x720.
     *
     * format=yuv420p makes the pixel format suitable
     * for many V4L2 applications.
     */
    arguments << "-vf"
              << "scale=1280:720,format=yuv420p";

    // Explicitly select YUV420P.
    arguments << "-pix_fmt" << "yuv420p";

    // Output format is Video4Linux2.
    arguments << "-f" << "v4l2";

    // Our virtual camera.
    arguments << m_outputDevice;

    /*
     * Start FFmpeg.
     *
     * "ffmpeg" assumes FFmpeg is available in PATH.
     */
    m_process.start("ffmpeg", arguments);
}


/*
 * Stop FFmpeg.
 */
void FFmpegProcess::stop()
{
    if (!isRunning())
    {
        return;
    }

    /*
     * First ask FFmpeg to terminate normally.
     */
    m_process.terminate();

    /*
     * Wait up to 2 seconds for FFmpeg to exit.
     */
    if (!m_process.waitForFinished(2000))
    {
        /*
         * If FFmpeg didn't stop normally,
         * forcefully kill it.
         */
        m_process.kill();

        m_process.waitForFinished(1000);
    }
}


/*
 * Return true when FFmpeg is running.
 */
bool FFmpegProcess::isRunning() const
{
    return m_process.state() == QProcess::Running;
}