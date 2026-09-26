#include "HardwareAcceleration.h"

#include <QProcess>

QString HardwareAcceleration::methodName(Method method)
{
    switch (method)
    {
    case Auto: return "Auto";
    case Software: return "Software";
    case VAAPI: return "VAAPI";
    case CUDA: return "CUDA";
    case QSV: return "QSV";
    case VDPAU: return "VDPAU";
    }

    return "Unknown";
}

QStringList HardwareAcceleration::detectAvailable(
    const QString &ffmpegPath)
{
    QProcess process;

    process.start(
        ffmpegPath,
        QStringList() << "-hide_banner" << "-hwaccels"
    );

    if (!process.waitForFinished(3000))
        return {};

    QString output =
        QString::fromLocal8Bit(
            process.readAllStandardOutput()
        );

    QStringList result;

    for (QString line :
         output.split('\n', Qt::SkipEmptyParts))
    {
        line = line.trimmed().toLower();

        if (line == "vaapi")
            result << "VAAPI";
        else if (line == "cuda")
            result << "CUDA";
        else if (line == "qsv")
            result << "QSV";
        else if (line == "vdpau")
            result << "VDPAU";
    }

    result.removeDuplicates();
    return result;
}

bool HardwareAcceleration::isAvailable(
    Method method,
    const QString &ffmpegPath)
{
    if (method == Auto || method == Software)
        return true;

    return detectAvailable(ffmpegPath)
        .contains(methodName(method));
}

HardwareAcceleration::Method
HardwareAcceleration::autoDetect(
    const QString &ffmpegPath)
{
    const QStringList available =
        detectAvailable(ffmpegPath);

    if (available.contains("VAAPI"))
        return VAAPI;

    if (available.contains("CUDA"))
        return CUDA;

    if (available.contains("QSV"))
        return QSV;

    if (available.contains("VDPAU"))
        return VDPAU;

    return Software;
}
