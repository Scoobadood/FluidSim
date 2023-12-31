#include "fluid_display_rgb_widget.h"
#include "grid_fluid_rgb_simulator.h"

#include <QCoreApplication>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QHBoxLayout>
#include <QPainter>
#include <QPushButton>
#include <QTimer>
#include <iostream>
#include <spdlog/spdlog.h>

const uint32_t HEIGHT = 512;
const uint32_t WIDTH = 512;

FluidDisplayRGBWidget::FluidDisplayRGBWidget(QWidget *parent)
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
    scene_image_buffer_ = new QImage(WIDTH, HEIGHT, QImage::Format_RGBA8888);
    std::memcpy(scene_image_buffer_->bits(), scene_image_->bits(), scene_image_->sizeInBytes());

    view_ = new QGraphicsView(this);
    view_->setRenderHint(QPainter::Antialiasing);

    scene_ = new QGraphicsScene(view_);
    view_->setScene(scene_);

    scene_->addPixmap(QPixmap::fromImage(*scene_image_));

    setLayout(new QVBoxLayout());
    layout()->addWidget(view_);

    setMouseTracking(true);

    // Set timer to update UI.
    auto ui_update_timer = new QTimer(this);
    connect(ui_update_timer, &QTimer::timeout, this, &FluidDisplayRGBWidget::UpdateUI);
    ui_update_timer->start(33); // 30fps (1000ms / 30fps)
}

void FluidDisplayRGBWidget::UpdateUI()
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

void FluidDisplayRGBWidget::SimulatorUpdated(const FluidSimulator2D *simulator)
{
    auto sim_x = simulator->DimX();
    auto sim_y = simulator->DimY();
    auto tile_x = scene_image_->width() / sim_x;
    auto tile_y = scene_image_->height() / sim_y;

    // Write to buffer
    QPainter painter(scene_image_buffer_);

    if (show_density_) {
      const float *src_red = simulator->Density().data();
      const float *src_grn = ((GridFluidRGBSimulator *) simulator)->DensityGreen().data();
      const float *src_blu = ((GridFluidRGBSimulator *) simulator)->DensityBlue().data();
      auto i = 0;
      for (auto y = 0; y < sim_y; ++y) {
	for (auto x = 0; x < sim_x; ++x) {
	  auto dst_red = (uint8_t) (std::fminf(255.0f,
					       std::fmaxf(0.0f, std::roundf(src_red[i] * 255.0f))));
	  auto dst_grn = (uint8_t) (std::fminf(255.0f,
					       std::fmaxf(0.0f, std::roundf(src_grn[i] * 255.0f))));
	  auto dst_blu = (uint8_t) (std::fminf(255.0f,
					       std::fmaxf(0.0f, std::roundf(src_blu[i] * 255.0f))));
	  painter.fillRect(tile_x * x,
			   tile_y * y,
			   tile_x,
			   tile_y,
			   QColor::fromRgb(dst_red, dst_grn, dst_blu, 255));
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
        painter.setBrush(QBrush(QColorConstants::Red, Qt::SolidPattern));

        for (auto y = 0; y < sim_y; ++y) {
            for (auto x = 0; x < sim_x; ++x) {
                auto vx = vel_x[i];
                auto vy = vel_y[i];
                auto start_x = tile_x * (x + 0.5f);
                auto start_y = tile_y * (y + 0.5f);
                auto end_x = tile_x * (x + 0.5f + vx);
                auto end_y = tile_y * (y + 0.5f + vy);

                auto vec_x = (end_x - start_x);
                auto vec_y = (end_y - start_y);
                auto perp_x = -vec_y * 0.125f;
                auto perp_y = vec_x * 0.125f;

                auto mid_x = start_x + (vec_x * 0.75f);
                auto mid_y = start_y + (vec_y * 0.75f);

                painter.drawLine(start_x, start_y, mid_x, mid_y);

                QPolygon arrowHead;
                arrowHead << QPoint(mid_x + perp_x, mid_y + perp_y) << QPoint(end_x, end_y)
                          << QPoint(mid_x - perp_x, mid_y - perp_y);
                painter.drawConvexPolygon(arrowHead);
                ++i;
            }
        }
    }
    if (painter.isActive())
        painter.end();

    // Copy buffer to scene.
    QMutexLocker locker(&scene_image_mutex_);
    std::memcpy(scene_image_->bits(), scene_image_buffer_->bits(), scene_image_->sizeInBytes());
    locker.unlock();
}

void FluidDisplayRGBWidget::ShowDensityField(bool show)
{
    show_density_ = show;
}

void FluidDisplayRGBWidget::ShowVelocityField(bool show)
{
    show_velocity_ = show;
    update();
}

void FluidDisplayRGBWidget::mousePressEvent(QMouseEvent *event)
{
    std::cout << "Mouse down at " << event->pos().x() << ", " << event->pos().y() << std::endl;
    auto view_pos = view_->mapFromParent(event->pos());
    std::cout << "  pos in view is " << view_pos.x() << ", " << view_pos.y() << std::endl;
    auto scene_pos = view_->mapToScene(view_pos);
    std::cout << "  pos in scene is " << scene_pos.x() << ", " << scene_pos.y() << std::endl;
    auto item = scene_->itemAt(scene_pos, QTransform());
    if (item) {
        if (event->button() == Qt::LeftButton) {
            auto pct_x = scene_pos.x() / WIDTH;
            auto pct_y = scene_pos.y() / HEIGHT;
            emit RightClick(pct_x, pct_y);
        }
    }
    // What we need here is the grid size so we can convert into a grid X,Y
    // But we don't own that, it's a simulator attribute.
    // We could convert the mouse click to a percentile image position click
    // and emit a signal and then have the MainWindow update the simulator.
    // Seems good to me.

    std::cout << "  mapped to scene " << scene_pos.x() << ", " << scene_pos.y() << std::endl;
    //        mapFromScene();
    // Convert the mouse coords to image coords in our thingo
}

FluidDisplayRGBWidget::~FluidDisplayRGBWidget() {}
