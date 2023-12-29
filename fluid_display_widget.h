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

public slots:
    void SimulatorUpdated(const FluidSimulator2D *simulator);
private slots:
    void UpdateUI();

private:
    QGraphicsScene *scene_;
    QImage *scene_image_;
    QMutex scene_image_mutex_;
};

#endif // FLUID_DISPLAY_WIDGET_H
