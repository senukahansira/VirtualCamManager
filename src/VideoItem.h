#ifndef VIDEOITEM_H
#define VIDEOITEM_H

#include <QString>
#include <QPixmap>

class VideoItem
{
public:
    explicit VideoItem(const QString &filePath);

    QString filePath() const;
    QString fileName() const;

    QString thumbnailPath() const;
    QPixmap thumbnail() const;
    void setThumbnail(const QPixmap &pixmap);
    bool hasThumbnail() const;

    qint64 durationMs() const;
    void setDurationMs(qint64 duration);

private:
    QString m_filePath;
    QString m_thumbnailPath;
    QPixmap m_thumbnail;
    qint64 m_durationMs;
};

#endif
