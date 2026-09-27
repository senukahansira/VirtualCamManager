#include "ThumbnailGenerator.h"

#include <QProcess>
#include <QFile>

ThumbnailGenerator::ThumbnailGenerator(QObject *parent)
    : QObject(parent)
{
}

void ThumbnailGenerator::generate(
    const QString &videoPath,
    const QString &outputPath)
{
    if (QFile::exists(outputPath))
    {
        QPixmap pixmap(outputPath);

        if (!pixmap.isNull())
        {
            emit thumbnailReady(videoPath, pixmap);
            return;
        }
    }

    const QStringList arguments = {
        "-hide_banner",
        "-loglevel", "error",
        "-ss", "1",
        "-i", videoPath,
        "-frames:v", "1",
        "-vf", "scale=320:-1",
        "-y", outputPath
    };

    QProcess *process = new QProcess(this);

    connect(
        process,
        QOverload<int, QProcess::ExitStatus>::of(
            &QProcess::finished),
        this,
        [this, process, videoPath, outputPath]
        (int exitCode, QProcess::ExitStatus)
        {
            if (exitCode != 0)
            {
                emit error(
                    videoPath,
                    "FFmpeg could not generate thumbnail."
                );
                process->deleteLater();
                return;
            }

            QPixmap pixmap(outputPath);

            if (pixmap.isNull())
            {
                emit error(
                    videoPath,
                    "Thumbnail could not be loaded."
                );
            }
            else
            {
                emit thumbnailReady(videoPath, pixmap);
            }

            process->deleteLater();
        }
    );

    connect(
        process,
        &QProcess::errorOccurred,
        this,
        [this, process, videoPath]
        (QProcess::ProcessError)
        {
            emit error(
                videoPath,
                "Could not start FFmpeg for thumbnail generation."
            );
            process->deleteLater();
        }
    );

    process->start("ffmpeg", arguments);
}
