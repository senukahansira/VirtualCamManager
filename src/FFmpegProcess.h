#ifndef FFMPEGPROCESS_H
#define FFMPEGPROCESS_H

#include <QObject>
#include <QProcess>
#include <QString>

/*
 * FFmpegProcess
 *
 * This class controls the FFmpeg program.
 *
 * Instead of manually typing:
 *
 * ffmpeg -nostdin -re ...
 *
 * our C++ program will execute FFmpeg for us.
 */
class FFmpegProcess : public QObject
{
    Q_OBJECT

public:

    explicit FFmpegProcess(QObject *parent = nullptr);

    // Start FFmpeg with the selected video.
    void start(const QString &videoPath);

    // Stop FFmpeg.
    void stop();

    // Check whether FFmpeg is currently running.
    bool isRunning() const;

signals:

    /*
     * These signals allow other classes, especially
     * MainWindow, to know what happened.
     */

    // Emitted when FFmpeg starts.
    void started();

    // Emitted when FFmpeg stops.
    void stopped();

    // Emitted when FFmpeg reports an error.
    void errorOccurred(const QString &message);

    // Emitted when FFmpeg writes information to stderr.
    void outputMessage(const QString &message);

private:

    // QProcess allows C++ to start external programs.
    QProcess m_process;

    // Our virtual camera device.
    QString m_outputDevice;

    // Prevent copying this class accidentally.
    Q_DISABLE_COPY(FFmpegProcess)
};

#endif // FFMPEGPROCESS_H