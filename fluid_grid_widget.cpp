#include "fluid_grid_widget.h"

#include <QPainter>
#include <QThread>
#include <QTimer>

/*

    Every time the laser scanner performs one measurement, you have four data to process: x, y (scanner position), phi (angle) and r (measured distance).
    This results in a single straight line starting at (x,y) with direction phi and length r, and a black dot at the end, as shown in the images.
    Everything else in the image stays the same. This is easily doable, also with QPainter. Just create a buffer QPixmap which is initially filled gray once.
    Then, at every scanner iteration, use drawLine and drawEllipse to draw the line and black dot respectively. These two operations are extremely fast, much
    faster than painting 16 million pixels individually.
    In the paint event of your widget, create a QPainter(this) as you did previously, and now use drawPixmap to draw the visible portion (->scrollbars) of the
    pixmap onto your widget surface. This, too, is very fast.
*/

FluidGridWidget::FluidGridWidget(QWidget *parent)
    : QWidget{parent} //
{
    setStyleSheet("background-color: lightblue;");

    fluid_grid_ = std::make_shared<FluidGrid>(400,400);

    image_ = std::make_shared<QImage>(400,400, QImage::Format_ARGB32);

    // Create a timer to update the UI at 30fps
    timer_ = std::make_shared<QTimer>(this);
    connect(timer_.get(), &QTimer::timeout, this, &FluidGridWidget::updateUI);
    timer_->start(1000 / 30); // 30fps

    // Animate the fluid
    fluid_grid_thread_ = std::make_shared<QThread>();
    fluid_grid_->moveToThread(fluid_grid_thread_.get());

    QObject::connect(fluid_grid_thread_.get(), &QThread::started, fluid_grid_.get(), &FluidGrid::Simulate);
    QObject::connect(fluid_grid_thread_.get(), &QThread::finished, fluid_grid_.get(), &QObject::deleteLater);
    QObject::connect(fluid_grid_thread_.get(), &QThread::finished, fluid_grid_thread_.get(), &QThread::deleteLater);

    fluid_grid_thread_->start();
}

void FluidGridWidget::paintEvent(QPaintEvent* event)    {
    QPainter painter(this);
//    // Update the image with the new data
//    QColor backgroundColor(Qt::white);
//    auto *ipainter = new QPainter(image_.get());
//    ipainter->fillRect(image_->rect(), backgroundColor);
//    ipainter->end();

    image_->setPixelColor(fluid_grid_->X(), fluid_grid_->Y(), Qt::red);
    painter.drawImage(0, 0, *image_);// Render the image on the widget
}

void FluidGridWidget::updateUI()
{
    // Trigger a repaint event to update the UI
    update();
}


