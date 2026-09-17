#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QPushButton;
class QVBoxLayout;
class QWidget;

class VideoManager;
class VirtualCameraManager;

/*
 * MainWindow
 *
 * This class controls the graphical user interface.
 *
 * It displays:
 *
 * - Virtual camera status
 * - Start/Stop Camera buttons
 * - Add Video button
 * - Video list
 * - Play/Stop/Remove buttons
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);

private slots:

    // Detect the VirtualCam device.
    void detectCamera();

    // Start the camera/playback system.
    void startCamera();

    // Stop the camera/playback system.
    void stopCamera();

    // Add a new video.
    void addVideo();

    // Rebuild the video list on screen.
    void refreshVideoList();

    // Called when a video starts.
    void onVideoStarted(int index);

    // Called when playback stops.
    void onVideoStopped();

    // Show an error.
    void showError(const QString &message);

private:

    /*
     * Create the GUI.
     */
    void setupUi();

    /*
     * Add one video row/card to the GUI.
     */
    QWidget *createVideoWidget(int index);

    /*
     * Update status labels.
     */
    void updateStatus();

    // Main video manager.
    VideoManager *m_videoManager;

    // Virtual camera detector.
    VirtualCameraManager *m_cameraManager;

    // Camera status text.
    QLabel *m_cameraStatusLabel;

    // Device path text.
    QLabel *m_deviceLabel;

    // General status text.
    QLabel *m_statusLabel;

    // Container for video rows.
    QWidget *m_videoContainer;

    // Layout containing video rows.
    QVBoxLayout *m_videoLayout;

    // Start camera button.
    QPushButton *m_startButton;

    // Stop camera button.
    QPushButton *m_stopButton;

    // Add video button.
    QPushButton *m_addButton;
};

#endif // MAINWINDOW_H