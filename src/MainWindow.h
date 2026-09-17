#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVector>
#include <QFrame>

#include "VideoManager.h"

/*
 * MainWindow
 *
 * This is the main GUI window.
 *
 * It contains:
 *
 * - Add Video button
 * - Video list
 * - Play buttons
 * - Stop buttons
 * - Remove buttons
 * - Status information
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);

private slots:

    // Called when user clicks "Add Video".
    void addVideo();

    // Rebuild the video list.
    void refreshVideoList();

    // Play a particular video.
    void playVideo(int index);

    // Stop a particular video.
    void stopVideo(int index);

    // Remove a particular video.
    void removeVideo(int index);

    // Update the status label.
    void updateStatus(const QString &message);

    // Display an error.
    void showError(const QString &message);

private:

    // Create the basic GUI.
    void setupUi();

    // Creates one row/card for one video.
    QWidget *createVideoWidget(int index);

    // Our video manager.
    VideoManager m_videoManager;

    // Main central widget.
    QWidget *m_centralWidget;

    // Main vertical layout.
    QVBoxLayout *m_mainLayout;

    // Area where video rows are displayed.
    QScrollArea *m_scrollArea;

    // Widget inside the scroll area.
    QWidget *m_videoContainer;

    // Layout inside video container.
    QVBoxLayout *m_videoLayout;

    // Add button.
    QPushButton *m_addButton;

    // Start virtual camera button.
    QPushButton *m_startCameraButton;

    // Stop virtual camera button.
    QPushButton *m_stopCameraButton;

    // Shows current status.
    QLabel *m_statusLabel;
};

#endif // MAINWINDOW_H