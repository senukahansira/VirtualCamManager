#include "VideoItem.h"

#include <QFileInfo>
#include <QDir>

VideoItem::VideoItem(const QString &filePath)
    : m_filePath(filePath),
      m_durationMs(0)
{
    const QString cacheDirectory =
        QDir::homePath() + "/.cache/VirtualCamManager";

    QDir().mkpath(cacheDirectory);

    QFileInfo info(filePath);

    m_thumbnailPath =
        cacheDirectory + "/" +
        info.fileName() +
        ".thumbnail.jpg";
}

QString VideoItem::filePath() const
{
    return m_filePath;
}

QString VideoItem::fileName() const
{
    return QFileInfo(m_filePath).fileName();
}

QString VideoItem::thumbnailPath() const
{
    return m_thumbnailPath;
}

QPixmap VideoItem::thumbnail() const
{
    return m_thumbnail;
}

void VideoItem::setThumbnail(const QPixmap &pixmap)
{
    m_thumbnail = pixmap;
}

bool VideoItem::hasThumbnail() const
{
    return !m_thumbnail.isNull();
}

qint64 VideoItem::durationMs() const
{
    return m_durationMs;
}

void VideoItem::setDurationMs(qint64 duration)
{
    m_durationMs = duration;
}
