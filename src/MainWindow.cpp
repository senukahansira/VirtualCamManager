#include "MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QFont>

/*
 * Constructor.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_centralWidget(nullptr),
      m_mainLayout(nullptr),
      m_scrollArea(nullptr),
      m_videoContainer(nullptr),
      m_videoLayout(nullptr),
      m_addButton(nullptr),
      m_startCameraButton(nullptr),
      m_stopCameraButton(nullptr),
      m_statusLabel(nullptr)
{
    /*
     * Create our GUI.
     */
    setupUi();

    /*
     * When the video list changes,
     * rebuild the GUI list.
     */
    connect(&m_videoManager,
            &VideoManager::videosChanged,
            this,
            &MainWindow::refreshVideoList);

    /*
     * Receive status messages.
     */
    connect(&m_videoManager,
            &VideoManager::statusMessage,
            this,
            &MainWindow::updateStatus);

    /*
     * Receive error messages.
     */
    connect(&m_videoManager,
            &VideoManager::errorMessage,
            this,
            &MainWindow::showError);

    /*
     * Set initial window size.
     */
    resize(900, 650);

    /*
     * Window title.
     */
    setWindowTitle("Virtual Camera Manager");

    /*
     * Create the initial empty video list.
     */
    refreshVideoList();
}


/*
 * Create the GUI.
 */
void MainWindow::setupUi()
{
    /*
     * Main central widget.
     */
    m_centralWidget = new QWidget(this);

    setCentralWidget(m_centralWidget);

    /*
     * Main layout.
     *
     * Everything is arranged vertically.
     */
    m_mainLayout = new QVBoxLayout(m_centralWidget);

    m_mainLayout->setContentsMargins(15, 15, 15, 15);

    m_mainLayout->setSpacing(10);


    /*
     * =====================================================
     * TITLE
     * =====================================================
     */

    QLabel *titleLabel = new QLabel(
        "Virtual Camera Manager",
        this
    );

    /*
     * Make title bigger.
     */
    QFont titleFont;

    titleFont.setPointSize(20);
    titleFont.setBold(true);

    titleLabel->setFont(titleFont);

    m_mainLayout->addWidget(titleLabel);


    /*
     * =====================================================
     * CAMERA CONTROL AREA
     * =====================================================
     */

    QHBoxLayout *cameraLayout = new QHBoxLayout();

    /*
     * Display the virtual camera device.
     */
    QLabel *cameraLabel = new QLabel(
        "Virtual Camera: /dev/video0",
        this
    );

    cameraLayout->addWidget(cameraLabel);

    /*
     * Spacer pushes buttons to the right.
     */
    cameraLayout->addStretch();


    /*
     * Start camera button.
     *
     * NOTE:
     *
     * v4l2loopback itself is loaded using modprobe.
     * This first version expects you to load it before
     * starting the application.
     */
    m_startCameraButton = new QPushButton(
        "Start Camera",
        this
    );

    cameraLayout->addWidget(m_startCameraButton);


    /*
     * Stop camera button.
     *
     * For this first version this stops the FFmpeg output.
     */
    m_stopCameraButton = new QPushButton(
        "Stop Camera",
        this
    );

    cameraLayout->addWidget(m_stopCameraButton);

    m_mainLayout->addLayout(cameraLayout);


    /*
     * =====================================================
     * ADD VIDEO BUTTON
     * =====================================================
     */

    QHBoxLayout *addLayout = new QHBoxLayout();

    m_addButton = new QPushButton(
        "+ Add Video",
        this
    );

    /*
     * Connect button click to addVideo().
     */
    connect(m_addButton,
            &QPushButton::clicked,
            this,
            &MainWindow::addVideo);

    addLayout->addWidget(m_addButton);

    addLayout->addStretch();

    m_mainLayout->addLayout(addLayout);


    /*
     * =====================================================
     * VIDEO LIST
     * =====================================================
     */

    /*
     * QScrollArea allows us to have more videos than
     * physically fit on the screen.
     */
    m_scrollArea = new QScrollArea(this);

    m_scrollArea->setWidgetResizable(true);

    /*
     * Container widget.
     */
    m_videoContainer = new QWidget();

    /*
     * Layout inside container.
     */
    m_videoLayout = new QVBoxLayout(m_videoContainer);

    m_videoLayout->setContentsMargins(5, 5, 5, 5);

    m_videoLayout->setSpacing(8);

    /*
     * Add a stretch at the bottom.
     *
     * This keeps video cards at the top.
     */
    m_videoLayout->addStretch();

    /*
     * Put container inside scroll area.
     */
    m_scrollArea->setWidget(m_videoContainer);

    /*
     * Add scroll area to main window.
     */
    m_mainLayout->addWidget(m_scrollArea);


    /*
     * =====================================================
     * STATUS
     * =====================================================
     */

    m_statusLabel = new QLabel(
        "Ready. Add a video.",
        this
    );

    m_statusLabel->setFrameStyle(
        QFrame::StyledPanel |
        QFrame::Sunken
    );

    m_mainLayout->addWidget(m_statusLabel);


    /*
     * =====================================================
     * CAMERA BUTTONS
     * =====================================================
     *
     * Start Camera:
     *
     * In this version we don't load the kernel module
     * automatically. The user should run:
     *
     * sudo modprobe v4l2loopback \
     *      devices=1 \
     *      video_nr=10 \
     *      card_label="VirtualCam" \
     *      exclusive_caps=1
     *
     * Then FFmpeg writes to /dev/video0.
     *
     * So clicking Start Camera simply displays a status.
     */

    connect(
        m_startCameraButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            updateStatus(
                "Virtual camera ready. "
                "Select a video and press Play."
            );
        }
    );


    /*
     * Stop Camera stops FFmpeg.
     */
    connect(
        m_stopCameraButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_videoManager.stopVideo();

            updateStatus(
                "Virtual camera output stopped."
            );
        }
    );
}


/*
 * User clicked "Add Video".
 */
void MainWindow::addVideo()
{
    /*
     * Open Linux/Qt file selection dialog.
     */
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Select Video",
        QString(),
        "Video Files (*.mp4 *.mkv *.avi *.mov *.webm *.flv *.m4v);;"
        "All Files (*)"
    );

    /*
     * User pressed Cancel.
     */
    if (filePath.isEmpty())
    {
        return;
    }

    /*
     * Give file to VideoManager.
     */
    m_videoManager.addVideo(filePath);
}


/*
 * Rebuild the video list.
 *
 * We call this whenever a video is added or removed.
 */
void MainWindow::refreshVideoList()
{
    /*
     * Remove all existing widgets from the video layout.
     */
    while (m_videoLayout->count() > 1)
    {
        QLayoutItem *item = m_videoLayout->takeAt(0);

        if (item->widget())
        {
            /*
             * deleteLater safely deletes the widget.
             */
            item->widget()->deleteLater();
        }

        delete item;
    }


    /*
     * Create a GUI card for each video.
     */
    for (int i = 0;
         i < m_videoManager.videoCount();
         ++i)
    {
        QWidget *videoWidget =
            createVideoWidget(i);

        /*
         * Insert before our bottom stretch.
         */
        m_videoLayout->insertWidget(
            m_videoLayout->count() - 1,
            videoWidget
        );
    }


    /*
     * Update Add Video button.
     */
    if (m_videoManager.videoCount()
        >= VideoManager::MAX_VIDEOS)
    {
        m_addButton->setEnabled(false);

        m_addButton->setText(
            "Maximum 10 Videos"
        );
    }
    else
    {
        m_addButton->setEnabled(true);

        m_addButton->setText(
            QString("+ Add Video (%1/10)")
                .arg(m_videoManager.videoCount())
        );
    }
}


/*
 * Create one GUI card for one video.
 */
QWidget *MainWindow::createVideoWidget(int index)
{
    /*
     * Get video object.
     */
    VideoItem *video =
        m_videoManager.videoAt(index);

    /*
     * Safety check.
     */
    if (!video)
    {
        return new QWidget();
    }


    /*
     * Create frame around video.
     */
    QFrame *frame = new QFrame(this);

    frame->setFrameStyle(
        QFrame::StyledPanel |
        QFrame::Raised
    );

    /*
     * Horizontal layout:
     *
     * [number] [filename]     [Play] [Stop] [Remove]
     */
    QHBoxLayout *layout =
        new QHBoxLayout(frame);

    layout->setContentsMargins(
        10, 8, 10, 8
    );


    /*
     * Video number.
     */
    QLabel *numberLabel =
        new QLabel(
            QString("%1.").arg(index + 1),
            frame
        );

    QFont numberFont;

    numberFont.setBold(true);

    numberLabel->setFont(numberFont);

    numberLabel->setMinimumWidth(30);

    layout->addWidget(numberLabel);


    /*
     * Filename label.
     */
    QLabel *nameLabel =
        new QLabel(
            video->fileName(),
            frame
        );

    /*
     * Allow filename to take available space.
     */
    nameLabel->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Preferred
    );

    /*
     * Tooltip shows complete path when mouse
     * hovers over filename.
     */
    nameLabel->setToolTip(
        video->filePath()
    );

    layout->addWidget(nameLabel);


    /*
     * PLAY BUTTON
     */
    QPushButton *playButton =
        new QPushButton(
            "▶ Play",
            frame
        );

    layout->addWidget(playButton);


    /*
     * STOP BUTTON
     */
    QPushButton *stopButton =
        new QPushButton(
            "■ Stop",
            frame
        );

    layout->addWidget(stopButton);


    /*
     * REMOVE BUTTON
     */
    QPushButton *removeButton =
        new QPushButton(
            "Remove",
            frame
        );

    layout->addWidget(removeButton);


    /*
     * =====================================================
     * BUTTON SIGNALS
     * =====================================================
     *
     * We capture "index" here.
     *
     * Example:
     *
     * Video 1 -> index 0
     * Video 2 -> index 1
     * Video 3 -> index 2
     */


    /*
     * Play button.
     */
    connect(
        playButton,
        &QPushButton::clicked,
        this,
        [this, index]()
        {
            playVideo(index);
        }
    );


    /*
     * Stop button.
     */
    connect(
        stopButton,
        &QPushButton::clicked,
        this,
        [this, index]()
        {
            stopVideo(index);
        }
    );


    /*
     * Remove button.
     */
    connect(
        removeButton,
        &QPushButton::clicked,
        this,
        [this, index]()
        {
            removeVideo(index);
        }
    );


    return frame;
}


/*
 * Play a video.
 */
void MainWindow::playVideo(int index)
{
    VideoItem *video =
        m_videoManager.videoAt(index);

    if (!video)
    {
        return;
    }

    /*
     * Tell the manager to start FFmpeg.
     */
    m_videoManager.playVideo(index);

    /*
     * Show status.
     */
    updateStatus(
        QString("Playing: %1")
            .arg(video->fileName())
    );
}


/*
 * Stop a video.
 */
void MainWindow::stopVideo(int index)
{
    Q_UNUSED(index);

    /*
     * There is only one FFmpeg output,
     * therefore Stop stops the current output.
     */
    m_videoManager.stopVideo();
}


/*
 * Remove a video.
 */
void MainWindow::removeVideo(int index)
{
    VideoItem *video =
        m_videoManager.videoAt(index);

    if (!video)
    {
        return;
    }


    /*
     * Ask the user for confirmation.
     */
    QMessageBox::StandardButton result =
        QMessageBox::question(
            this,
            "Remove Video",
            QString("Remove \"%1\"?")
                .arg(video->fileName()),
            QMessageBox::Yes |
            QMessageBox::No
        );


    if (result == QMessageBox::Yes)
    {
        m_videoManager.removeVideo(index);
    }
}


/*
 * Update status label.
 */
void MainWindow::updateStatus(
    const QString &message)
{
    m_statusLabel->setText(message);
}


/*
 * Show an error message.
 */
void MainWindow::showError(
    const QString &message)
{
    m_statusLabel->setText(
        "Error: " + message
    );

    /*
     * Also show a popup.
     */
    QMessageBox::warning(
        this,
        "Virtual Camera Manager",
        message
    );
}