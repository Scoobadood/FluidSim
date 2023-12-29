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
    : QWidget(parent)       //
    , show_density_{true}   //
    , show_velocity_{false} //
{
    scene_image_ = new QImage(WIDTH, HEIGHT, QImage::Format_RGBA8888);
    QPainter painter(scene_image_);
    painter.fillRect(0, 0, WIDTH, HEIGHT, QColor(Qt::white)); // Fill the image with a white background
    painter.setPen(Qt::black);
    painter.drawText(10, 20, "Sim"); // Draw text on the image
    painter.end();

    view_ = new QGraphicsView(this);
    view_->setRenderHint(QPainter::Antialiasing);
    view_->setRenderHint(QPainter::SmoothPixmapTransform);

    scene_ = new QGraphicsScene(view_);
    view_->setScene(scene_);

    scene_->addPixmap(QPixmap::fromImage(*scene_image_));

    setLayout(new QVBoxLayout());
    layout()->addWidget(view_);

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

    auto widthScaleFactor = width() / scene_->width();
    auto heightScaleFactor = height() / scene_->height();
    auto scaleFactor = qMin(widthScaleFactor, heightScaleFactor);

    // Scale the view's transformation matrix
    view_->setTransform(QTransform::fromScale(scaleFactor, scaleFactor));
}

void FluidDisplayWidget::SimulatorUpdated(const FluidSimulator2D *simulator)
{
    const float *src = simulator->Density().data();
    auto sim_x = simulator->DimX();
    auto sim_y = simulator->DimY();
    auto tile_x = scene_image_->width() / sim_x;
    auto tile_y = scene_image_->height() / sim_y;

    QMutexLocker locker(&scene_image_mutex_);
    QPainter painter(scene_image_);

    if (show_density_) {
        auto i = 0;
        for (auto y = 0; y < sim_y; ++y) {
            for (auto x = 0; x < sim_x; ++x) {
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
    } else {
        painter.fillRect(0, 0, sim_x * tile_x, sim_y * tile_y, QColorConstants::Black);
    }

    // Get data from Simulator and update display
    if (show_velocity_) {
        auto vel_x = simulator->VelocityX();
        auto vel_y = simulator->VelocityY();

        auto i = 0;
        painter.setPen(QColorConstants::Red);
        for (auto y = 0; y < sim_y; ++y) {
            for (auto x = 0; x < sim_x; ++x) {
                auto vx = vel_x[i];
                auto vy = vel_y[i];
                auto start_x = tile_x * (x + 0.5f);
                auto start_y = tile_y * (y + 0.5f);
                auto end_x = tile_x * (x + 0.5f + vx);
                auto end_y = tile_y * (y + 0.5f + vy);
                painter.drawLine(start_x, start_y, end_x, end_y);
                auto mid_x = (end_x - start_x) * 0.5f;
                auto mid_y = (end_y - start_y) * 0.5f;
                QPolygon arrowHead;
                arrowHead << QPoint(start_x + mid_x - mid_y, start_y + mid_y - mid_x)
                          << QPoint(end_x, end_y)
                          << QPoint(start_x + mid_x + mid_y, start_y + mid_y + mid_x);
                painter.drawPolygon(arrowHead);
                ++i;
            }
        }
    }
    painter.end();
    locker.unlock();
}

void FluidDisplayWidget::ShowDensityField(bool show)
{
    show_density_ = show;
}
void FluidDisplayWidget::ShowVelocityField(bool show)
{
    show_velocity_ = show;
    update();
}

FluidDisplayWidget::~FluidDisplayWidget() {}
