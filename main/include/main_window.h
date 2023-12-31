#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "fluid_display_rgb_widget.h"
#include "grid_fluid_rgb_simulator.h"
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

  void HandleRightClick(float px, float py);

protected:
private:
  FluidSimulatorThread *sim_thread_;
  GridFluidRGBSimulator *fluid_sim_;
  FluidDisplayRGBWidget *display_;
};

#endif // MAIN_WINDOW_H
