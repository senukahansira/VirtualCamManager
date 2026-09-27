#ifndef FFMPEGPROCESS_H
#define FFMPEGPROCESS_H

#include <QObject>
#include <QProcess>

#include "HardwareAcceleration.h"

class FFmpegProcess : public QObject
{
    Q_OBJECT

public:
    explicit FFmpegProcess(QObject *parent = nullptr);

    void start(
        const QString &videoPath,
        qint64 startPositionMs = 0
    );

    void stop();
    void pause();
    void resume();
    void seek(qint64 positionMs);

    void setLoop(bool loop);

    void setHardwareAcceleration(
        HardwareAcceleration::Method method
    );

    bool isRunning() const;
    bool isPaused() const;

    qint64 positionMs() const;
    qint64 durationMs() const;

signals:
    void started();
    void stopped();
    void paused();
    void resumed();

    void positionChanged(qint64 positionMs);
    void durationChanged(qint64 durationMs);

    void errorOccurred(const QString &message);
    void logMessage(const QString &message);

private:
    void buildAndStartProcess();
    void parseFFmpegOutput(const QString &text);
    QStringList createArguments() const;
    QString createVideoFilter() const;

    QProcess m_process;

    QString m_videoPath;
    QString m_outputDevice;

    qint64 m_positionMs;
    qint64 m_durationMs;
    qint64 m_startPositionMs;

    bool m_loop;
    bool m_paused;

    HardwareAcceleration::Method m_hardwareAcceleration;
};

#endif
