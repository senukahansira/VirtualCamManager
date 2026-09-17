#ifndef VIDEOITEM_H
#define VIDEOITEM_H

#include <QString>

/*
 * VideoItem
 *
 * This is a very small data class.
 *
 * It represents ONE video in our application.
 *
 * Example:
 *
 *     Name: sandy.mp4
 *     Path: /home/user/Downloads/sandy.mp4
 */
class VideoItem
{
public:

    // Constructor.
    VideoItem(const QString &path = QString());

    // Return the complete file path.
    QString path() const;

    // Return only the filename.
    QString name() const;

private:

    // Complete path to the video file.
    QString m_path;
};

#endif // VIDEOITEM_H