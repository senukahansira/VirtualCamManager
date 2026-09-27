#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSlider>
#include <QComboBox>
#include <QCheckBox>

#include "VideoManager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(
        QWidget *parent = nullptr
    );

private slots:
    void addVideo();
    void refreshVideoList();

    void playVideo(int index);
    void removeVideo(int index);

    void updatePosition(qint64 position);
    void updateDuration(qint64 duration);

    void togglePause();
    void seekVideo(int value);

    void hardwareAccelerationChanged(
        int index
    );

private:
    void setupUi();

    QWidget *createVideoWidget(
        int index
    );

    QString formatTime(
        qint64 milliseconds
    ) const;

    VideoManager m_videoManager;

    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;

    QPushButton *m_startCameraButton;
    QPushButton *m_stopCameraButton;
    QPushButton *m_addButton;

    QScrollArea *m_scrollArea;
    QWidget *m_videoContainer;
    QVBoxLayout *m_videoLayout;

    QPushButton *m_pauseButton;
    QPushButton *m_stopButton;

    QSlider *m_seekSlider;
    QLabel *m_timeLabel;

    QComboBox *m_hardwareCombo;
    QCheckBox *m_loopCheckBox;

    QLabel *m_statusLabel;

    bool m_updatingSlider;
};

#endif