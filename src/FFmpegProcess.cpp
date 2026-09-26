#include "FFmpegProcess.h"

#include <QRegularExpression>

FFmpegProcess::FFmpegProcess(QObject *parent)
    : QObject(parent),
      m_outputDevice("/dev/video10"),
      m_positionMs(0),
      m_durationMs(0),
      m_startPositionMs(0),
      m_loop(true),
      m_paused(false),
      m_hardwareAcceleration(
          HardwareAcceleration::Auto)
{
    connect(
        &m_process,
        &QProcess::readyReadStandardError,
        this,
        [this]()
        {
            const QString text =
                QString::fromLocal8Bit(
                    m_process.readAllStandardError());

            parseFFmpegOutput(text);
            emit logMessage(text);
        }
    );

    connect(
        &m_process,
        &QProcess::started,
        this,
        [this]()
        {
            emit started();
        }
    );

    connect(
        &m_process,
        QOverload<int, QProcess::ExitStatus>::of(
            &QProcess::finished),
        this,
        [this](int, QProcess::ExitStatus)
        {
            if (!m_paused)
                emit stopped();
        }
    );

    connect(
        &m_process,
        &QProcess::errorOccurred,
        this,
        [this](QProcess::ProcessError error)
        {
            QString message;

            switch (error)
            {
            case QProcess::FailedToStart:
                message =
                    "FFmpeg failed to start. "
                    "Make sure FFmpeg is installed.";
                break;

            case QProcess::Crashed:
                message = "FFmpeg crashed.";
                break;

            default:
                message = "FFmpeg process error.";
                break;
            }

            emit errorOccurred(message);
        }
    );
}

void FFmpegProcess::start(
    const QString &videoPath,
    qint64 startPositionMs)
{
    stop();

    m_videoPath = videoPath;
    m_startPositionMs = qMax<qint64>(0, startPositionMs);
    m_positionMs = m_startPositionMs;
    m_paused = false;

    buildAndStartProcess();
}

void FFmpegProcess::buildAndStartProcess()
{
    emit logMessage("Starting FFmpeg...");
    m_process.start("ffmpeg", createArguments());
}

QStringList FFmpegProcess::createArguments() const
{
    QStringList args;

    args << "-nostdin"
         << "-hide_banner"
         << "-progress" << "pipe:2"
         << "-re";

    if (m_startPositionMs > 0)
    {
        const double seconds =
            static_cast<double>(m_startPositionMs) / 1000.0;

        args << "-ss"
             << QString::number(seconds, 'f', 3);
    }

    if (m_loop)
        args << "-stream_loop" << "-1";

    args << "-i" << m_videoPath;

    HardwareAcceleration::Method method =
        m_hardwareAcceleration;

    if (method == HardwareAcceleration::Auto)
        method = HardwareAcceleration::autoDetect();

    if (method == HardwareAcceleration::VAAPI)
    {
        args << "-vaapi_device"
             << "/dev/dri/renderD128";
    }
    else if (method == HardwareAcceleration::CUDA)
    {
        args << "-hwaccel" << "cuda";
    }
    else if (method == HardwareAcceleration::QSV)
    {
        args << "-hwaccel" << "qsv";
    }
    else if (method == HardwareAcceleration::VDPAU)
    {
        args << "-hwaccel" << "vdpau";
    }

    args << "-vf" << createVideoFilter()
         << "-pix_fmt" << "yuv420p"
         << "-f" << "v4l2"
         << m_outputDevice;

    return args;
}

QString FFmpegProcess::createVideoFilter() const
{
    return "scale=1280:720,format=yuv420p";
}

void FFmpegProcess::stop()
{
    if (m_process.state() == QProcess::NotRunning)
        return;

    m_paused = false;

    m_process.terminate();

    if (!m_process.waitForFinished(1500))
    {
        m_process.kill();
        m_process.waitForFinished(1000);
    }

    emit stopped();
}

void FFmpegProcess::pause()
{
    if (!isRunning())
        return;

    m_startPositionMs = m_positionMs;
    m_paused = true;

    m_process.terminate();

    if (!m_process.waitForFinished(1000))
    {
        m_process.kill();
        m_process.waitForFinished(500);
    }

    emit paused();
}

void FFmpegProcess::resume()
{
    if (!m_paused)
        return;

    m_paused = false;

    buildAndStartProcess();

    emit resumed();
}

void FFmpegProcess::seek(qint64 positionMs)
{
    if (m_videoPath.isEmpty())
        return;

    if (m_durationMs > 0)
    {
        positionMs =
            qBound<qint64>(
                0,
                positionMs,
                m_durationMs
            );
    }
    else
    {
        positionMs = qMax<qint64>(0, positionMs);
    }

    m_positionMs = positionMs;
    m_startPositionMs = positionMs;

    if (m_paused)
    {
        emit positionChanged(m_positionMs);
        return;
    }

    const bool wasRunning = isRunning();

    if (wasRunning)
    {
        m_paused = true;
        m_process.terminate();

        if (!m_process.waitForFinished(1000))
        {
            m_process.kill();
            m_process.waitForFinished(500);
        }

        m_paused = false;
    }

    buildAndStartProcess();

    emit positionChanged(m_positionMs);
}

void FFmpegProcess::setLoop(bool loop)
{
    m_loop = loop;
}

void FFmpegProcess::setHardwareAcceleration(
    HardwareAcceleration::Method method)
{
    m_hardwareAcceleration = method;
}

bool FFmpegProcess::isRunning() const
{
    return m_process.state() == QProcess::Running;
}

bool FFmpegProcess::isPaused() const
{
    return m_paused;
}

qint64 FFmpegProcess::positionMs() const
{
    return m_positionMs;
}

qint64 FFmpegProcess::durationMs() const
{
    return m_durationMs;
}

void FFmpegProcess::parseFFmpegOutput(
    const QString &text)
{
    /*
     * FFmpeg -progress reports out_time_us.
     * Use it when available for reliable progress.
     */
    static const QRegularExpression
        outTimeRegex(
            "out_time_us=([0-9]+)"
        );

    const QRegularExpressionMatch
        timeMatch =
            outTimeRegex.match(text);

    if (timeMatch.hasMatch())
    {
        bool ok = false;

        const qint64 microseconds =
            timeMatch.captured(1).toLongLong(&ok);

        if (ok)
        {
            const qint64 relativeMs =
                microseconds / 1000;

            m_positionMs =
                m_startPositionMs + relativeMs;

            emit positionChanged(m_positionMs);
        }
    }

    static const QRegularExpression
        durationRegex(
            "Duration:\\s*"
            "([0-9]+):([0-9]+):([0-9]+)"
            "\\.([0-9]+)"
        );

    const QRegularExpressionMatch
        durationMatch =
            durationRegex.match(text);

    if (durationMatch.hasMatch())
    {
        bool ok1 = false;
        bool ok2 = false;
        bool ok3 = false;
        bool ok4 = false;

        const qint64 hours =
            durationMatch.captured(1)
                .toLongLong(&ok1);

        const qint64 minutes =
            durationMatch.captured(2)
                .toLongLong(&ok2);

        const qint64 seconds =
            durationMatch.captured(3)
                .toLongLong(&ok3);

        const qint64 fraction =
            durationMatch.captured(4)
                .left(3)
                .toLongLong(&ok4);

        if (ok1 && ok2 && ok3 && ok4)
        {
            m_durationMs =
                hours * 3600000 +
                minutes * 60000 +
                seconds * 1000 +
                fraction;

            emit durationChanged(m_durationMs);
        }
    }
}
