#include "MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFrame>
#include <QFont>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_centralWidget(nullptr),
      m_mainLayout(nullptr),
      m_startCameraButton(nullptr),
      m_stopCameraButton(nullptr),
      m_addButton(nullptr),
      m_scrollArea(nullptr),
      m_videoContainer(nullptr),
      m_videoLayout(nullptr),
      m_pauseButton(nullptr),
      m_stopButton(nullptr),
      m_seekSlider(nullptr),
      m_timeLabel(nullptr),
      m_hardwareCombo(nullptr),
      m_loopCheckBox(nullptr),
      m_statusLabel(nullptr),
      m_updatingSlider(false)
{
    setupUi();

    connect(
        &m_videoManager,
        &VideoManager::videosChanged,
        this,
        &MainWindow::refreshVideoList
    );

    connect(
        &m_videoManager,
        &VideoManager::positionChanged,
        this,
        &MainWindow::updatePosition
    );

    connect(
        &m_videoManager,
        &VideoManager::durationChanged,
        this,
        &MainWindow::updateDuration
    );

    connect(
        &m_videoManager,
        &VideoManager::statusMessage,
        this,
        [this](const QString &message)
        {
            m_statusLabel->setText(message);
        }
    );

    connect(
        &m_videoManager,
        &VideoManager::errorMessage,
        this,
        [this](const QString &message)
        {
            m_statusLabel->setText(
                "Error: " + message
            );

            QMessageBox::warning(
                this,
                "Virtual Camera Manager",
                message
            );
        }
    );

    refreshVideoList();

    resize(1000, 750);

    setWindowTitle(
        "Virtual Camera Manager 2.0"
    );
}

void MainWindow::setupUi()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout =
        new QVBoxLayout(m_centralWidget);

    m_mainLayout->setContentsMargins(
        15, 15, 15, 15
    );

    m_mainLayout->setSpacing(10);

    QLabel *title =
        new QLabel(
            "Virtual Camera Manager 2.0",
            this
        );

    QFont titleFont;
    titleFont.setPointSize(20);
    titleFont.setBold(true);

    title->setFont(titleFont);

    m_mainLayout->addWidget(title);

    /*
     * Camera controls.
     */
    QHBoxLayout *cameraLayout =
        new QHBoxLayout();

    cameraLayout->addWidget(
        new QLabel(
            "Virtual Camera: /dev/video10",
            this
        )
    );

    cameraLayout->addStretch();

    m_startCameraButton =
        new QPushButton(
            "Start Camera",
            this
        );

    m_stopCameraButton =
        new QPushButton(
            "Stop Camera",
            this
        );

    cameraLayout->addWidget(
        m_startCameraButton
    );

    cameraLayout->addWidget(
        m_stopCameraButton
    );

    m_mainLayout->addLayout(
        cameraLayout
    );

    connect(
        m_startCameraButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_statusLabel->setText(
                "Camera ready. Select a video and press Play."
            );
        }
    );

    connect(
        m_stopCameraButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_videoManager.stop();
        }
    );

    /*
     * Add video.
     */
    QHBoxLayout *addLayout =
        new QHBoxLayout();

    m_addButton =
        new QPushButton(
            "+ Add Video",
            this
        );

    addLayout->addWidget(m_addButton);
    addLayout->addStretch();

    connect(
        m_addButton,
        &QPushButton::clicked,
        this,
        &MainWindow::addVideo
    );

    m_mainLayout->addLayout(addLayout);

    /*
     * Video list.
     */
    m_scrollArea =
        new QScrollArea(this);

    m_scrollArea->setWidgetResizable(true);

    m_videoContainer =
        new QWidget();

    m_videoLayout =
        new QVBoxLayout(
            m_videoContainer
        );

    m_videoLayout->setSpacing(8);
    m_videoLayout->addStretch();

    m_scrollArea->setWidget(
        m_videoContainer
    );

    m_mainLayout->addWidget(
        m_scrollArea
    );

    /*
     * Playback controls.
     */
    QFrame *controlFrame =
        new QFrame(this);

    controlFrame->setFrameStyle(
        QFrame::StyledPanel |
        QFrame::Raised
    );

    QVBoxLayout *controlLayout =
        new QVBoxLayout(controlFrame);

    QHBoxLayout *controls =
        new QHBoxLayout();

    m_pauseButton =
        new QPushButton(
            "▶ Play / ❚❚ Pause",
            this
        );

    m_stopButton =
        new QPushButton(
            "■ Stop",
            this
        );

    controls->addWidget(m_pauseButton);
    controls->addWidget(m_stopButton);

    m_loopCheckBox =
        new QCheckBox(
            "Loop",
            this
        );

    m_loopCheckBox->setChecked(true);

    controls->addWidget(m_loopCheckBox);

    controls->addWidget(
        new QLabel("Hardware:", this)
    );

    m_hardwareCombo =
        new QComboBox(this);

    m_hardwareCombo->addItem(
        "Auto",
        static_cast<int>(
            HardwareAcceleration::Auto
        )
    );

    m_hardwareCombo->addItem(
        "Software",
        static_cast<int>(
            HardwareAcceleration::Software
        )
    );

    m_hardwareCombo->addItem(
        "VAAPI",
        static_cast<int>(
            HardwareAcceleration::VAAPI
        )
    );

    m_hardwareCombo->addItem(
        "CUDA",
        static_cast<int>(
            HardwareAcceleration::CUDA
        )
    );

    m_hardwareCombo->addItem(
        "QSV",
        static_cast<int>(
            HardwareAcceleration::QSV
        )
    );

    m_hardwareCombo->addItem(
        "VDPAU",
        static_cast<int>(
            HardwareAcceleration::VDPAU
        )
    );

    controls->addWidget(
        m_hardwareCombo
    );

    controls->addStretch();

    controlLayout->addLayout(controls);

    /*
     * Seek bar.
     */
    QHBoxLayout *seekLayout =
        new QHBoxLayout();

    m_timeLabel =
        new QLabel(
            "00:00 / 00:00",
            this
        );

    seekLayout->addWidget(
        m_timeLabel
    );

    m_seekSlider =
        new QSlider(
            Qt::Horizontal,
            this
        );

    m_seekSlider->setRange(0, 0);

    seekLayout->addWidget(
        m_seekSlider
    );

    controlLayout->addLayout(
        seekLayout
    );

    m_mainLayout->addWidget(
        controlFrame
    );

    /*
     * Signals.
     */
    connect(
        m_pauseButton,
        &QPushButton::clicked,
        this,
        &MainWindow::togglePause
    );

    connect(
        m_stopButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_videoManager.stop();
        }
    );

    connect(
        m_seekSlider,
        &QSlider::sliderMoved,
        this,
        &MainWindow::seekVideo
    );

    connect(
        m_seekSlider,
        &QSlider::sliderReleased,
        this,
        [this]()
        {
            seekVideo(m_seekSlider->value());
        }
    );

    connect(
        m_loopCheckBox,
        &QCheckBox::toggled,
        this,
        [this](bool checked)
        {
            m_videoManager.setLoop(checked);
        }
    );

    connect(
        m_hardwareCombo,
        QOverload<int>::of(
            &QComboBox::currentIndexChanged
        ),
        this,
        &MainWindow::hardwareAccelerationChanged
    );

    m_statusLabel =
        new QLabel(
            "Ready.",
            this
        );

    m_statusLabel->setFrameStyle(
        QFrame::StyledPanel |
        QFrame::Sunken
    );

    m_mainLayout->addWidget(
        m_statusLabel
    );
}

void MainWindow::addVideo()
{
    const QString filePath =
        QFileDialog::getOpenFileName(
            this,
            "Select Video",
            QString(),
            "Video Files (*.mp4 *.mkv *.avi *.mov *.webm *.flv *.m4v);;"
            "All Files (*)"
        );

    if (filePath.isEmpty())
        return;

    m_videoManager.addVideo(filePath);
}

void MainWindow::refreshVideoList()
{
    while (m_videoLayout->count() > 1)
    {
        QLayoutItem *item =
            m_videoLayout->takeAt(0);

        if (item->widget())
            item->widget()->deleteLater();

        delete item;
    }

    for (int i = 0;
         i < m_videoManager.videoCount();
         ++i)
    {
        QWidget *widget =
            createVideoWidget(i);

        m_videoLayout->insertWidget(
            m_videoLayout->count() - 1,
            widget
        );
    }

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
            QString(
                "+ Add Video (%1/10)"
            ).arg(
                m_videoManager.videoCount()
            )
        );
    }
}

QWidget *MainWindow::createVideoWidget(
    int index)
{
    VideoItem *video =
        m_videoManager.videoAt(index);

    if (!video)
        return new QWidget();

    QFrame *frame =
        new QFrame(this);

    frame->setFrameStyle(
        QFrame::StyledPanel |
        QFrame::Raised
    );

    QHBoxLayout *layout =
        new QHBoxLayout(frame);

    QLabel *thumbnail =
        new QLabel(frame);

    thumbnail->setFixedSize(160, 90);
    thumbnail->setAlignment(Qt::AlignCenter);
    thumbnail->setStyleSheet(
        "background:#202020;"
    );

    if (video->hasThumbnail())
    {
        thumbnail->setPixmap(
            video->thumbnail()
                .scaled(
                    160,
                    90,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                )
        );
    }
    else
    {
        thumbnail->setText("Generating...");
    }

    layout->addWidget(thumbnail);

    QLabel *number =
        new QLabel(
            QString("%1.")
                .arg(index + 1),
            frame
        );

    QFont numberFont;
    numberFont.setBold(true);
    number->setFont(numberFont);
    number->setMinimumWidth(30);

    layout->addWidget(number);

    QLabel *name =
        new QLabel(
            video->fileName(),
            frame
        );

    name->setToolTip(
        video->filePath()
    );

    name->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Preferred
    );

    layout->addWidget(name);

    QPushButton *play =
        new QPushButton(
            "▶ Play",
            frame
        );

    QPushButton *remove =
        new QPushButton(
            "Remove",
            frame
        );

    layout->addWidget(play);
    layout->addWidget(remove);

    connect(
        play,
        &QPushButton::clicked,
        this,
        [this, index]()
        {
            playVideo(index);
        }
    );

    connect(
        remove,
        &QPushButton::clicked,
        this,
        [this, index]()
        {
            removeVideo(index);
        }
    );

    return frame;
}

void MainWindow::playVideo(int index)
{
    VideoItem *video =
        m_videoManager.videoAt(index);

    if (!video)
        return;

    m_videoManager.playVideo(index);

    m_updatingSlider = true;

    m_seekSlider->setValue(0);

    m_seekSlider->setRange(
        0,
        static_cast<int>(
            qMax<qint64>(
                0,
                video->durationMs()
            )
        )
    );

    m_updatingSlider = false;

    m_statusLabel->setText(
        QString("Playing: %1")
            .arg(video->fileName())
    );
}

void MainWindow::removeVideo(int index)
{
    VideoItem *video =
        m_videoManager.videoAt(index);

    if (!video)
        return;

    const auto result =
        QMessageBox::question(
            this,
            "Remove Video",
            QString(
                "Remove \"%1\"?"
            ).arg(video->fileName()),
            QMessageBox::Yes |
            QMessageBox::No
        );

    if (result == QMessageBox::Yes)
        m_videoManager.removeVideo(index);
}

void MainWindow::updatePosition(qint64 position)
{
    if (m_updatingSlider)
        return;

    m_updatingSlider = true;

    const int value =
        static_cast<int>(
            qBound<qint64>(
                static_cast<qint64>(0),
                position,
                static_cast<qint64>(
                    m_seekSlider->maximum()
                )
            )
        );

    m_seekSlider->setValue(value);

    m_updatingSlider = false;

    m_timeLabel->setText(
        formatTime(position) +
        " / " +
        formatTime(
            m_seekSlider->maximum()
        )
    );
}

void MainWindow::updateDuration(
    qint64 duration)
{
    m_updatingSlider = true;

    m_seekSlider->setRange(
        0,
        static_cast<int>(
            qMax<qint64>(0, duration)
        )
    );

    m_updatingSlider = false;

    m_timeLabel->setText(
        formatTime(
            m_seekSlider->value()
        ) +
        " / " +
        formatTime(duration)
    );
}

void MainWindow::togglePause()
{
    if (m_videoManager.isPaused())
    {
        m_videoManager.resume();
        return;
    }

    if (m_videoManager.isPlaying())
    {
        m_videoManager.pause();
        return;
    }

    const int index =
        m_videoManager.currentVideoIndex();

    if (index >= 0)
    {
        m_videoManager.playVideo(index);
    }
    else if (m_videoManager.videoCount() > 0)
    {
        m_videoManager.playVideo(0);
    }
}

void MainWindow::seekVideo(int value)
{
    if (m_updatingSlider)
        return;

    m_videoManager.seek(
        static_cast<qint64>(value)
    );
}

void MainWindow::hardwareAccelerationChanged(
    int index)
{
    const QVariant data =
        m_hardwareCombo->itemData(index);

    if (!data.isValid())
        return;

    const auto method =
        static_cast<
            HardwareAcceleration::Method
        >(data.toInt());

    m_videoManager
        .setHardwareAcceleration(method);

    m_statusLabel->setText(
        "Hardware acceleration: " +
        HardwareAcceleration::methodName(method)
    );
}

QString MainWindow::formatTime(
    qint64 milliseconds) const
{
    const qint64 totalSeconds =
        milliseconds / 1000;

    const qint64 hours =
        totalSeconds / 3600;

    const qint64 minutes =
        (totalSeconds % 3600) / 60;

    const qint64 seconds =
        totalSeconds % 60;

    if (hours > 0)
    {
        return QString("%1:%2:%3")
            .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    }

    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}
