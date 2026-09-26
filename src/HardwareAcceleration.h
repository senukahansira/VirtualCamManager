#ifndef HARDWAREACCELERATION_H
#define HARDWAREACCELERATION_H

#include <QString>
#include <QStringList>

class HardwareAcceleration
{
public:
    enum Method
    {
        Auto,
        Software,
        VAAPI,
        CUDA,
        QSV,
        VDPAU
    };

    static QString methodName(Method method);

    static QStringList detectAvailable(
        const QString &ffmpegPath = "ffmpeg"
    );

    static bool isAvailable(
        Method method,
        const QString &ffmpegPath = "ffmpeg"
    );

    static Method autoDetect(
        const QString &ffmpegPath = "ffmpeg"
    );
};

#endif
