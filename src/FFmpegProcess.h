#ifndef FFMPEGPROCESS_H
#define FFMPEGPROCESS_H

#include <QObject>
#include <QProcess>
#include <QString>

/*
 * FFmpegProcess
 *
 * This class is responsible ONLY for starting and stopping FFmpeg.
 *
 * It does not manage the video list.
 * It does not manage the GUI.
 * It simply says:
 *
 *     "Run this video through FFmpeg into this V4L2 device."
 */
class FFmpegProcess : public QObject
{
    Q_OBJECT

public:
    explicit FFmpegProcess(QObject *parent = nullptr);

    // Start playing a video into the specified V4L2 device.
    void start(const QString &videoPath, const QString &devicePath);

    // Stop the currently running FFmpeg process.
    void stop();

    // Returns true if FFmpeg is currently running.
    bool isRunning() const;

signals:

    // Emitted when FFmpeg starts successfully.
    void started();

    // Emitted when FFmpeg stops.
    void stopped();

    // Emitted when FFmpeg reports an error.
    void errorOccurred(const QString &message);

    // Emitted when FFmpeg prints output.
    void outputReceived(const QString &output);

private:

    // QProcess is Qt's way of starting and controlling another program.
    QProcess *m_process;
};

#endif // FFMPEGPROCESS_H