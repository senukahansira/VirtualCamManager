#ifndef VIDEOITEM_H
#define VIDEOITEM_H

#include <QString>

/*
 * VideoItem represents ONE video in our video list.
 *
 * For example:
 *
 * VideoItem
 *     name     = "ll.mp4"
 *     filePath = "/home/user/Videos/ll.mp4"
 */
class VideoItem
{
public:

    // Constructor.
    VideoItem(const QString &filePath);

    // Return the complete path to the video.
    QString filePath() const;

    // Return only the filename.
    // Example: "ll.mp4"
    QString fileName() const;

private:

    // Full path to the video file.
    QString m_filePath;
};

#endif // VIDEOITEM_H