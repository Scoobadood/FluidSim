#ifndef FLUID_DISPLAY_WIDGET_H
#define FLUID_DISPLAY_WIDGET_H

#include "fluid_simulator_thread.h"

#include <QGraphicsView>
#include <QMutex>
#include <QPushButton>

class FluidDisplayWidget : public QWidget {
Q_OBJECT

public:
  explicit FluidDisplayWidget(QWidget *parent = nullptr);

  ~FluidDisplayWidget() override;

  void mousePressEvent(QMouseEvent *event) override;

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"

  void SpawnSource(float px, float py);

#pragma clang diagnostic pop


public slots:

  void SimulatorUpdated(const FluidSimulator2D *simulator);

  void ShowDensityField(bool);

  void ShowVelocityField(bool);

private slots:

  void UpdateUI();

private:
  bool show_density_;
  bool show_velocity_;
  QGraphicsView *view_;
  QGraphicsScene *scene_;
  QImage *scene_image_;
  QImage *scene_image_buffer_;
  QMutex scene_image_mutex_;
};

#endif // FLUID_DISPLAY_WIDGET_H
