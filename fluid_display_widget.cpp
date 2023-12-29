#include "fluid_display_widget.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>

const uint32_t HEIGHT = 200;
const uint32_t WIDTH = 200;

FluidDisplayWidget::FluidDisplayWidget(QWidget *parent)
    : QWidget(parent) //
{
    scene_image_ = new QImage(WIDTH, HEIGHT, QImage::Format_Grayscale8);
    QPainter painter(scene_image_);
    painter.fillRect(0, 0, WIDTH, HEIGHT, QColor(Qt::white)); // Fill the image with a white background
    painter.setPen(Qt::black);
    painter.drawText(10, 20, "Sim"); // Draw text on the image
    painter.end();

    auto gw = new QGraphicsView(this);

    scene_ = new QGraphicsScene(gw);
    gw->setScene(scene_);
    scene_->addPixmap(QPixmap::fromImage(*scene_image_));

    setLayout(new QVBoxLayout());
    layout()->addWidget(gw);

    // Set timer to update UI.
    auto ui_update_timer = new QTimer(this);
    connect(ui_update_timer, &QTimer::timeout, this, &FluidDisplayWidget::UpdateUI);
    ui_update_timer->start(33); // 30fps (1000ms / 30fps)
}

void FluidDisplayWidget::UpdateUI()
{
    // Lock the mutex to access the data
    QMutexLocker locker(&scene_image_mutex_);

    scene_->clear();
    scene_->addPixmap(QPixmap::fromImage(*scene_image_));

    locker.unlock();
}

void FluidDisplayWidget::SimulatorUpdated(const FluidSimulator *simulator)
{
    // Get data from Simulator and update display
    QMutexLocker locker(&scene_image_mutex_);
    const float *src = simulator->Density().data();
    uint8_t *dst = scene_image_->bits();
    for (auto i = 0; i < HEIGHT * WIDTH; ++i) {
        dst[i] = (uint8_t) (std::fminf(255.0f, std::fmaxf(0.0f, std::roundf(src[i] * 255.0f))));
    }
    locker.unlock();
}

FluidDisplayWidget::~FluidDisplayWidget() {}
