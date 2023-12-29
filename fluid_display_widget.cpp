#include "fluid_display_widget.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QHBoxLayout>
#include <QPainter>
#include <QPushButton>
#include <QTimer>

const uint32_t HEIGHT = 1024;
const uint32_t WIDTH = 1024;

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

void FluidDisplayWidget::SimulatorUpdated(const FluidSimulator2D *simulator)
{
    // Get data from Simulator and update display
    QMutexLocker locker(&scene_image_mutex_);
    const float *src = simulator->Density().data();

    auto tile_x = scene_image_->width() / simulator->DimX();
    auto tile_y = scene_image_->height() / simulator->DimY();

    QPainter painter(scene_image_);
    auto i = 0;
    for (auto y = 0; y < simulator->DimY(); ++y) {
        for (auto x = 0; x < simulator->DimX(); ++x) {
            auto dst = (uint8_t) (std::fminf(255.0f,
                                             std::fmaxf(0.0f, std::roundf(src[i] * 255.0f))));
            painter.fillRect(tile_x * x,
                             tile_y * y,
                             tile_x,
                             tile_y,
                             QColor::fromRgb(dst, dst, dst, 255));
            ++i;
        }
    }
    painter.end();
    locker.unlock();
}

FluidDisplayWidget::~FluidDisplayWidget() {}
