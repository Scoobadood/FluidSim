#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "fluid_display_widget.h"
#include "fluid_sim_grid.h"
#include "fluid_simulator_thread.h"

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

public slots:
    void StepSim();
    void ResetSim();
    void StartSim();
    void StopSim();

protected:
private:
    FluidSimulatorThread *sim_thread_;
    GridFluidSimulator *fluid_sim_;
    FluidDisplayWidget *display_;
};

#endif // MAINWINDOW_H
