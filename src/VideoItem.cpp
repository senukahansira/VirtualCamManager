#include "VideoItem.h"

#include <QFileInfo>

/*
 * Constructor
 */
VideoItem::VideoItem(const QString &path)
    : m_path(path)
{
}


/*
 * Return the complete video path.
 */
QString VideoItem::path() const
{
    return m_path;
}


/*
 * Return only the filename.
 *
 * Example:
 *
 * /home/user/Downloads/sandy.mp4
 *
 * becomes:
 *
 * sandy.mp4
 */
QString VideoItem::name() const
{
    QFileInfo fileInfo(m_path);

    return fileInfo.fileName();
}