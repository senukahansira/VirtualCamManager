#ifndef VIRTUALCAMERAMANAGER_H
#define VIRTUALCAMERAMANAGER_H

#include <QObject>
#include <QString>

/*
 * VirtualCameraManager
 *
 * Its job is to find our v4l2loopback camera.
 *
 * Instead of assuming:
 *
 *     /dev/video0
 *
 * it checks the available /dev/video* devices and searches
 * for one whose V4L2 card name is "VirtualCam".
 */
class VirtualCameraManager : public QObject
{
    Q_OBJECT

public:

    explicit VirtualCameraManager(QObject *parent = nullptr);

    // Search for VirtualCam.
    bool detect();

    // Return the detected device path.
    QString devicePath() const;

    // Return the detected camera name.
    QString cameraName() const;

    // True when a camera was detected.
    bool isAvailable() const;

private:

    /*
     * Read the V4L2 card name from a device.
     *
     * Example:
     *
     * /dev/video0 -> VirtualCam
     */
    QString getV4L2CardName(const QString &devicePath) const;

    // Detected device path.
    QString m_devicePath;

    // Detected device name.
    QString m_cameraName;
};

#endif // VIRTUALCAMERAMANAGER_H