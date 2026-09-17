#include "MainWindow.h"

#include "VideoManager.h"
#include "VirtualCameraManager.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>


/*
 * Constructor
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_videoManager(new VideoManager(this)),
      m_cameraManager(new VirtualCameraManager(this))
{
    /*
     * Build the graphical interface.
     */
    setupUi();

    /*
     * Connect VideoManager signals to GUI slots.
     */
    connect(
        m_videoManager,
        &VideoManager::videosChanged,
        this,
        &MainWindow::refreshVideoList
    );

    connect(
        m_videoManager,
        &VideoManager::videoStarted,
        this,
        &MainWindow::onVideoStarted
    );

    connect(
        m_videoManager,
        &VideoManager::videoStopped,
        this,
        &MainWindow::onVideoStopped
    );

    connect(
        m_videoManager,
        &VideoManager::errorOccurred,
        this,
        &MainWindow::showError
    );

    /*
     * Try to find VirtualCam immediately when the application starts.
     */
    detectCamera();

    /*
     * Initially there are no videos.
     */
    refreshVideoList();
}


/*
 * Build the GUI.
 */
void MainWindow::setupUi()
{
    setWindowTitle("Virtual Camera Manager");

    /*
     * Give the main window a reasonable starting size.
     */
    resize(850, 650);

    /*
     * Central widget.
     */
    QWidget *centralWidget = new QWidget(this);

    setCentralWidget(centralWidget);

    /*
     * Main vertical layout.
     */
    QVBoxLayout *mainLayout =
        new QVBoxLayout(centralWidget);

    /*
     * -----------------------------
     * CAMERA SECTION
     * -----------------------------
     */

    QLabel *titleLabel =
        new QLabel("<h2>Virtual Camera Manager</h2>");

    mainLayout->addWidget(titleLabel);

    /*
     * Camera status.
     */
    m_cameraStatusLabel =
        new QLabel("Camera: Checking...");

    mainLayout->addWidget(
        m_cameraStatusLabel
    );

    /*
     * Device path.
     */
    m_deviceLabel =
        new QLabel("Device: Not detected");

    mainLayout->addWidget(
        m_deviceLabel
    );

    /*
     * Camera buttons.
     */
    QHBoxLayout *cameraButtonLayout =
        new QHBoxLayout();

    m_startButton =
        new QPushButton("Start Camera");

    m_stopButton =
        new QPushButton("Stop Camera");

    QPushButton *detectButton =
        new QPushButton("Detect Camera");

    cameraButtonLayout->addWidget(
        m_startButton
    );

    cameraButtonLayout->addWidget(
        m_stopButton
    );

    cameraButtonLayout->addWidget(
        detectButton
    );

    mainLayout->addLayout(
        cameraButtonLayout
    );

    /*
     * Connect buttons.
     */
    connect(
        m_startButton,
        &QPushButton::clicked,
        this,
        &MainWindow::startCamera
    );

    connect(
        m_stopButton,
        &QPushButton::clicked,
        this,
        &MainWindow::stopCamera
    );

    connect(
        detectButton,
        &QPushButton::clicked,
        this,
        &MainWindow::detectCamera
    );

    /*
     * -----------------------------
     * VIDEO SECTION
     * -----------------------------
     */

    QHBoxLayout *videoHeader =
        new QHBoxLayout();

    QLabel *videosLabel =
        new QLabel("<h3>Videos</h3>");

    m_addButton =
        new QPushButton("+ Add Video");

    videoHeader->addWidget(
        videosLabel
    );

    videoHeader->addStretch();

    videoHeader->addWidget(
        m_addButton
    );

    mainLayout->addLayout(
        videoHeader
    );

    /*
     * Add video button.
     */
    connect(
        m_addButton,
        &QPushButton::clicked,
        this,
        &MainWindow::addVideo
    );

    /*
     * Scroll area.
     *
     * This allows us to have up to 10 videos without
     * making the entire window enormous.
     */
    QScrollArea *scrollArea =
        new QScrollArea();

    scrollArea->setWidgetResizable(true);

    /*
     * Container inside the scroll area.
     */
    m_videoContainer =
        new QWidget();

    m_videoLayout =
        new QVBoxLayout(
            m_videoContainer
        );

    /*
     * Empty space at the bottom.
     */
    m_videoLayout->addStretch();

    scrollArea->setWidget(
        m_videoContainer
    );

    mainLayout->addWidget(
        scrollArea
    );

    /*
     * -----------------------------
     * STATUS
     * -----------------------------
     */

    m_statusLabel =
        new QLabel("Status: Ready");

    mainLayout->addWidget(
        m_statusLabel
    );

    /*
     * Initial button state.
     */
    m_startButton->setEnabled(false);
    m_stopButton->setEnabled(false);
}


/*
 * Detect VirtualCam.
 */
void MainWindow::detectCamera()
{
    /*
     * Ask VirtualCameraManager to search /dev/video*.
     */
    if (m_cameraManager->detect())
    {
        const QString device =
            m_cameraManager->devicePath();

        const QString name =
            m_cameraManager->cameraName();

        /*
         * Give the detected device to VideoManager.
         */
        m_videoManager->setCameraDevice(
            device
        );

        /*
         * Update GUI.
         */
        m_cameraStatusLabel->setText(
            "Camera: Connected (" + name + ")"
        );

        m_deviceLabel->setText(
            "Device: " + device
        );

        /*
         * Camera can now be started.
         */
        m_startButton->setEnabled(true);

        m_statusLabel->setText(
            "Status: Virtual camera detected."
        );
    }
    else
    {
        /*
         * No VirtualCam found.
         */
        m_videoManager->setCameraDevice(
            QString()
        );

        m_cameraStatusLabel->setText(
            "Camera: Not detected"
        );

        m_deviceLabel->setText(
            "Device: None"
        );

        m_startButton->setEnabled(false);

        m_statusLabel->setText(
            "Status: VirtualCam not found. "
            "Load v4l2loopback first."
        );
    }
}


/*
 * Start Camera button.
 */
void MainWindow::startCamera()
{
    /*
     * Make sure the camera still exists.
     *
     * It may have been unplugged/recreated after the application
     * started.
     */
    if (!m_cameraManager->detect())
    {
        showError(
            "VirtualCam was not found."
        );

        return;
    }

    /*
     * Update VideoManager with the latest device path.
     */
    m_videoManager->setCameraDevice(
        m_cameraManager->devicePath()
    );

    /*
     * If there are no videos, there is nothing for FFmpeg
     * to play yet.
     */
    if (m_videoManager->count() == 0)
    {
        m_statusLabel->setText(
            "Status: Camera ready. Add a video."
        );

        m_stopButton->setEnabled(false);

        return;
    }

    /*
     * Start the first video automatically.
     */
    if (m_videoManager->play(0))
    {
        m_startButton->setEnabled(false);
        m_stopButton->setEnabled(true);
    }
}


/*
 * Stop Camera button.
 */
void MainWindow::stopCamera()
{
    /*
     * Stop FFmpeg.
     */
    m_videoManager->stop();

    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);

    m_statusLabel->setText(
        "Status: Camera stopped."
    );
}


/*
 * Add Video button.
 */
void MainWindow::addVideo()
{
    /*
     * Check maximum number first.
     */
    if (m_videoManager->count() >=
        VideoManager::MAX_VIDEOS)
    {
        QMessageBox::information(
            this,
            "Video Limit",
            "You can add a maximum of 10 videos."
        );

        return;
    }

    /*
     * Open file picker.
     */
    const QString path =
        QFileDialog::getOpenFileName(
            this,
            "Select Video",
            QString(),
            "Video Files (*.mp4 *.mkv *.avi *.mov *.webm *.m4v);;All Files (*)"
        );

    /*
     * User cancelled.
     */
    if (path.isEmpty())
        return;

    /*
     * Add the selected video.
     */
    if (!m_videoManager->addVideo(path))
    {
        QMessageBox::warning(
            this,
            "Cannot Add Video",
            "The video could not be added.\n\n"
            "It may already be in the list or the "
            "maximum of 10 videos has been reached."
        );
    }
}


/*
 * Rebuild the video list.
 */
void MainWindow::refreshVideoList()
{
    /*
     * Remove all existing widgets from the layout.
     *
     * We rebuild the list because there are only a maximum
     * of 10 items, so this is simple and perfectly adequate.
     */

    while (m_videoLayout->count() > 1)
    {
        QLayoutItem *item =
            m_videoLayout->takeAt(0);

        if (!item)
            continue;

        QWidget *widget =
            item->widget();

        if (widget)
        {
            widget->deleteLater();
        }

        delete item;
    }

    /*
     * Create a widget for every video.
     */
    for (int i = 0;
         i < m_videoManager->count();
         ++i)
    {
        QWidget *videoWidget =
            createVideoWidget(i);

        /*
         * Insert before the stretch at the bottom.
         */
        m_videoLayout->insertWidget(
            i,
            videoWidget
        );
    }

    /*
     * Add button is disabled when 10 videos exist.
     */
    m_addButton->setEnabled(
        m_videoManager->count()
        < VideoManager::MAX_VIDEOS
    );

    updateStatus();
}


/*
 * Create one video row.
 */
QWidget *MainWindow::createVideoWidget(int index)
{
    QWidget *widget =
        new QWidget();

    /*
     * Give each video a border.
     */
    widget->setStyleSheet(
        "QWidget {"
        "border: 1px solid #555;"
        "border-radius: 6px;"
        "padding: 6px;"
        "}"
    );

    QHBoxLayout *layout =
        new QHBoxLayout(widget);

    /*
     * Video filename.
     */
    const VideoItem &video =
        m_videoManager->videos().at(index);

    QLabel *nameLabel =
        new QLabel(video.name());

    /*
     * Let the filename use available space.
     */
    nameLabel->setMinimumWidth(200);

    layout->addWidget(
        nameLabel
    );

    layout->addStretch();

    /*
     * PLAY button.
     */
    QPushButton *playButton =
        new QPushButton("Play");

    layout->addWidget(
        playButton
    );

    /*
     * STOP button.
     */
    QPushButton *stopButton =
        new QPushButton("Stop");

    layout->addWidget(
        stopButton
    );

    /*
     * REMOVE button.
     */
    QPushButton *removeButton =
        new QPushButton("Remove");

    layout->addWidget(
        removeButton
    );

    /*
     * Capture index safely.
     *
     * We store the index in a local variable.
     */
    connect(
        playButton,
        &QPushButton::clicked,
        this,
        [this, index]()
        {
            /*
             * Make sure VirtualCam still exists.
             */
            if (!m_cameraManager->detect())
            {
                showError(
                    "VirtualCam was not found."
                );

                return;
            }

            /*
             * Update device path.
             */
            m_videoManager->setCameraDevice(
                m_cameraManager->devicePath()
            );

            /*
             * Play selected video.
             */
            if (m_videoManager->play(index))
            {
                m_startButton->setEnabled(false);
                m_stopButton->setEnabled(true);
            }
        }
    );

    /*
     * STOP button.
     */
    connect(
        stopButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_videoManager->stop();

            m_startButton->setEnabled(true);
            m_stopButton->setEnabled(false);

            updateStatus();
        }
    );

    /*
     * REMOVE button.
     */
    connect(
        removeButton,
        &QPushButton::clicked,
        this,
        [this, index]()
        {
            m_videoManager->removeVideo(index);
        }
    );

    /*
     * Highlight currently playing video.
     */
    if (index ==
        m_videoManager->currentIndex())
    {
        widget->setStyleSheet(
            "QWidget {"
            "border: 2px solid #22aa55;"
            "border-radius: 6px;"
            "padding: 6px;"
            "}"
        );
    }

    return widget;
}


/*
 * Called when a video starts.
 */
void MainWindow::onVideoStarted(int index)
{
    /*
     * Make sure the index is valid.
     */
    if (index < 0 ||
        index >= m_videoManager->count())
    {
        return;
    }

    const QString name =
        m_videoManager->videos()
            .at(index)
            .name();

    m_statusLabel->setText(
        "Status: Playing " + name
    );

    m_startButton->setEnabled(false);
    m_stopButton->setEnabled(true);

    /*
     * Refresh list so the playing item gets highlighted.
     */
    refreshVideoList();
}


/*
 * Called when video playback stops.
 */
void MainWindow::onVideoStopped()
{
    m_statusLabel->setText(
        "Status: Playback stopped."
    );

    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);

    refreshVideoList();
}


/*
 * Display an error.
 */
void MainWindow::showError(
    const QString &message)
{
    m_statusLabel->setText(
        "Error: " + message
    );

    QMessageBox::warning(
        this,
        "Virtual Camera Error",
        message
    );
}


/*
 * Update general status.
 */
void MainWindow::updateStatus()
{
    if (m_videoManager->isPlaying())
    {
        const int index =
            m_videoManager->currentIndex();

        if (index >= 0 &&
            index < m_videoManager->count())
        {
            m_statusLabel->setText(
                "Status: Playing " +
                m_videoManager->videos()
                    .at(index)
                    .name()
            );

            return;
        }
    }

    if (m_cameraManager->isAvailable())
    {
        m_statusLabel->setText(
            "Status: Camera ready."
        );
    }
    else
    {
        m_statusLabel->setText(
            "Status: VirtualCam not detected."
        );
    }
}