#ifndef THUMBNAILGENERATOR_H
#define THUMBNAILGENERATOR_H

#include <QObject>
#include <QString>
#include <QPixmap>

class ThumbnailGenerator : public QObject
{
    Q_OBJECT

public:
    explicit ThumbnailGenerator(QObject *parent = nullptr);

    void generate(
        const QString &videoPath,
        const QString &outputPath
    );

signals:
    void thumbnailReady(
        const QString &videoPath,
        const QPixmap &pixmap
    );

    void error(
        const QString &videoPath,
        const QString &message
    );
};

#endif
