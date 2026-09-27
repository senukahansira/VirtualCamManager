#ifndef CLICKABLESLIDER_H
#define CLICKABLESLIDER_H

#include <QSlider>
#include <QMouseEvent>
#include <QStyle>

class ClickableSlider : public QSlider
{
    Q_OBJECT

public:
    explicit ClickableSlider(
        Qt::Orientation orientation,
        QWidget *parent = nullptr
    )
        : QSlider(orientation, parent)
    {
    }

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton)
        {
            const int position =
                static_cast<int>(
                    event->position().x()
                );

            const int value =
                QStyle::sliderValueFromPosition(
                    minimum(),
                    maximum(),
                    position,
                    width()
                );

            setSliderPosition(value);

            setSliderDown(true);

            emit sliderPressed();

            event->accept();
            return;
        }

        QSlider::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (isSliderDown())
        {
            const int position =
                qBound(
                    0,
                    static_cast<int>(
                        event->position().x()
                    ),
                    width()
                );

            const int value =
                QStyle::sliderValueFromPosition(
                    minimum(),
                    maximum(),
                    position,
                    width()
                );

            setSliderPosition(value);

            event->accept();
            return;
        }

        QSlider::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton &&
            isSliderDown())
        {
            setSliderDown(false);

            emit sliderReleased();

            event->accept();
            return;
        }

        QSlider::mouseReleaseEvent(event);
    }
};

#endif