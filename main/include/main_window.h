#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "fluid_display_widget.h"
#include "grid_fluid_simulator.h"
#include "fluid_simulator_thread.h"

#include <QMainWindow>

class MainWindow : public QMainWindow {
Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

public slots:

  void StepSim();

  void ResetSim();

  void StartSim();

  void StopSim();

  void HandleClick(float px, float py);

protected:
private:
  FluidSimulatorThread *sim_thread_;
  GridFluidSimulator *fluid_sim_;
  FluidDisplayWidget *display_;
};

#endif // MAIN_WINDOW_H
