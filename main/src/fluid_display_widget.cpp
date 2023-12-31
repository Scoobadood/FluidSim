#include "fluid_display_widget.h"

#include <QCoreApplication>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QHBoxLayout>
#include <QPainter>
#include <QPushButton>
#include <QTimer>
#include <iostream>
#include "spdlog/spdlog.h"

const uint32_t HEIGHT = 512;
const uint32_t WIDTH = 512;

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
  connect(ui_update_timer, &QTimer::timeout, this, &FluidDisplayWidget::UpdateUI);
  ui_update_timer->start(33); // 30fps (1000ms / 30fps)
}

void FluidDisplayWidget::UpdateUI() {
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

void FluidDisplayWidget::SimulatorUpdated(const FluidSimulator2D *simulator) {
  auto sim_x = simulator->DimX();
  auto sim_y = simulator->DimY();
  auto tile_x = scene_image_->width() / sim_x;
  auto tile_y = scene_image_->height() / sim_y;

  // Write to buffer
  QPainter painter(scene_image_buffer_);
  painter.fillRect(scene_image_buffer_->rect(), QColorConstants::Black);
  if (show_density_) {
    const float *src = simulator->Density().data();
    for (auto y = 1; y < sim_y - 1; ++y) {
      for (auto x = 1; x < sim_x - 1; ++x) {
        auto idx = y * sim_x + x;
        auto dst = (uint8_t) (std::fminf(255.0f,
                                         std::fmaxf(0.0f, std::roundf(src[idx] * 255.0f))));
        painter.fillRect((int32_t) tile_x * x,
                         (int32_t) tile_y * y,
                         (int32_t) tile_x,
                         (int32_t) tile_y,
                         QColor::fromRgb(dst, dst, dst, 255));
      }
    }
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
        auto start_x = (float) tile_x * ((float) x + 0.5f);
        auto start_y = (float) tile_y * ((float) y + 0.5f);
        auto end_x = (float) tile_x * ((float) x + 0.5f + vx);
        auto end_y = (float) tile_y * ((float) y + 0.5f + vy);

        auto vec_x = (end_x - start_x);
        auto vec_y = (end_y - start_y);
        auto perp_x = -vec_y * 0.125f;
        auto perp_y = vec_x * 0.125f;

        auto mid_x = start_x + (vec_x * 0.75f);
        auto mid_y = start_y + (vec_y * 0.75f);

        painter.drawLine((int32_t) std::roundf(start_x),
                         (int32_t) std::roundf(start_y),
                         (int32_t) std::roundf(mid_x),
                         (int32_t) std::roundf(mid_y));

        QPolygonF arrowHead;
        arrowHead << QPointF(mid_x + perp_x, mid_y + perp_y) << QPointF(end_x, end_y)
                  << QPointF(mid_x - perp_x, mid_y - perp_y);
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

void FluidDisplayWidget::ShowDensityField(bool show) {
  show_density_ = show;
}

void FluidDisplayWidget::ShowVelocityField(bool show) {
  show_velocity_ = show;
  update();
}

void FluidDisplayWidget::mousePressEvent(QMouseEvent *event) {
  std::cout << "Mouse down at " << event->pos().x() << ", " << event->pos().y() << std::endl;
  auto view_pos = view_->mapFromParent(event->pos());
  std::cout << "  pos in view is " << view_pos.x() << ", " << view_pos.y() << std::endl;
  auto scene_pos = view_->mapToScene(view_pos);
  std::cout << "  pos in scene is " << scene_pos.x() << ", " << scene_pos.y() << std::endl;
  auto item = scene_->itemAt(scene_pos, QTransform());
  if (item) {
    if (event->button() == Qt::LeftButton) {
      auto pct_x = (float)(scene_pos.x() / WIDTH);
      auto pct_y = (float)(scene_pos.y() / HEIGHT);
      emit SpawnSource(pct_x, pct_y);
    }
  }
}

FluidDisplayWidget::~FluidDisplayWidget() = default;
