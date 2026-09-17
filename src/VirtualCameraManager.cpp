#include "VirtualCameraManager.h"

#include <QDir>
#include <QFileInfo>
#include <QDebug>

#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>

/*
 * Constructor
 */
VirtualCameraManager::VirtualCameraManager(QObject *parent)
    : QObject(parent)
{
}


/*
 * Detect VirtualCam.
 */
bool VirtualCameraManager::detect()
{
    /*
     * Clear the previous result.
     */
    m_devicePath.clear();
    m_cameraName.clear();

    /*
     * Look inside /dev for video devices.
     *
     * This will find things such as:
     *
     * /dev/video0
     * /dev/video1
     * /dev/video2
     */
    QDir devDirectory("/dev");

    QStringList devices =
        devDirectory.entryList(
            QStringList() << "video*",
            QDir::System | QDir::Readable
        );

    /*
     * Check every video device.
     */
    for (const QString &deviceName : devices)
    {
        const QString path =
            "/dev/" + deviceName;

        /*
         * Ask Linux for the V4L2 card name.
         */
        const QString cardName =
            getV4L2CardName(path);

        qDebug()
            << "Checking camera:"
            << path
            << "name:"
            << cardName;

        /*
         * We specifically want our v4l2loopback device.
         *
         * The name comes from:
         *
         * card_label="VirtualCam"
         */
        if (cardName.compare(
                "VirtualCam",
                Qt::CaseInsensitive) == 0)
        {
            m_devicePath = path;
            m_cameraName = cardName;

            return true;
        }
    }

    /*
     * Nothing found.
     */
    return false;
}


/*
 * Read the V4L2 card name.
 */
QString VirtualCameraManager::getV4L2CardName(
    const QString &devicePath) const
{
    /*
     * Open the Linux video device.
     *
     * O_NONBLOCK prevents us from getting stuck waiting
     * for camera data.
     */
    const QByteArray path =
        devicePath.toLocal8Bit();

    int fd = open(
        path.constData(),
        O_RDWR | O_NONBLOCK
    );

    if (fd < 0)
    {
        return QString();
    }

    /*
     * Structure used by the V4L2 API to describe a device.
     */
    struct v4l2_capability capability;

    /*
     * Clear the structure before using it.
     */
    memset(&capability, 0, sizeof(capability));

    /*
     * Ask the driver for device information.
     */
    const int result =
        ioctl(
            fd,
            VIDIOC_QUERYCAP,
            &capability
        );

    /*
     * We no longer need the device.
     */
    close(fd);

    if (result < 0)
    {
        return QString();
    }

    /*
     * card contains the human-readable camera name.
     *
     * Example:
     *
     * "VirtualCam"
     *
     * or:
     *
     * "HP HD Camera"
     */
    return QString::fromLocal8Bit(
        reinterpret_cast<const char *>(
            capability.card
        )
    ).trimmed();
}


/*
 * Return detected device path.
 */
QString VirtualCameraManager::devicePath() const
{
    return m_devicePath;
}


/*
 * Return camera name.
 */
QString VirtualCameraManager::cameraName() const
{
    return m_cameraName;
}


/*
 * Return whether a camera was detected.
 */
bool VirtualCameraManager::isAvailable() const
{
    return !m_devicePath.isEmpty();
}