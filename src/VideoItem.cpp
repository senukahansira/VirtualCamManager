#include "VideoItem.h"

#include <QFileInfo>

/*
 * Constructor.
 *
 * We receive the video's full path and save it.
 */
VideoItem::VideoItem(const QString &filePath)
    : m_filePath(filePath)
{
}

/*
 * Return the full path.
 *
 * Example:
 *
 * /home/myuser/Videos/ll.mp4
 */
QString VideoItem::filePath() const
{
    return m_filePath;
}

/*
 * Return only the filename.
 *
 * QFileInfo removes the directory part.
 *
 * Example:
 *
 * /home/myuser/Videos/ll.mp4
 *
 * becomes:
 *
 * ll.mp4
 */
QString VideoItem::fileName() const
{
    QFileInfo info(m_filePath);

    return info.fileName();
}
