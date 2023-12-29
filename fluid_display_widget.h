#ifndef FLUID_DISPLAY_WIDGET_H
#define FLUID_DISPLAY_WIDGET_H

#include "fluid_simulator_thread.h"

#include <QGraphicsView>
#include <QMutex>
#include <QPushButton>

class FluidDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    FluidDisplayWidget(QWidget *parent = nullptr);
    ~FluidDisplayWidget();
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void RightClick(float px, float py);

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
    QMutex scene_image_mutex_;
};

#endif // FLUID_DISPLAY_WIDGET_H
