#include "main_window.h"
#include "control_panel_widget.h"
#include "fluid_display_rgb_widget.h"

#include <QDockWidget>

const uint32_t SIM_GRID_SIZE = 128;

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow{parent}  //
        , sim_thread_{nullptr} //
{
  auto control_panel = new ControlPanelWidget(this);

  // Create a QDockWidget
  auto dock = new QDockWidget("Controls", this);
  dock->setWidget(control_panel);
  addDockWidget(Qt::TopDockWidgetArea, dock);

  fluid_sim_ = new GridFluidRGBSimulator(SIM_GRID_SIZE, SIM_GRID_SIZE, 1.0f / 15.0f, 0.8f);

  // Add some central content to the main window
  display_ = new FluidDisplayRGBWidget(this);
  setCentralWidget(display_);

  connect(control_panel, &ControlPanelWidget::Start, this, &MainWindow::StartSim);
  connect(control_panel, &ControlPanelWidget::Stop, this, &MainWindow::StopSim);
  connect(control_panel, &ControlPanelWidget::Reset, this, &MainWindow::ResetSim);
  connect(control_panel, &ControlPanelWidget::Step, this, &MainWindow::StepSim);
  connect(control_panel, &ControlPanelWidget::ShowDensity, this, [&](bool show) {
    display_->ShowDensityField(show);
    StepSim();
  });
  connect(control_panel, &ControlPanelWidget::ShowVelocity, this, [&](bool show) {
    display_->ShowVelocityField(show);
    StepSim();
  });
  connect(display_, &FluidDisplayRGBWidget::SpawnSource, this, &MainWindow::HandleRightClick);
}

void MainWindow::StepSim() {
  // Guard to prevent calls to SimulatorUpdated() from multiple threads
  if (sim_thread_ && sim_thread_->isRunning())
    return;

  fluid_sim_->Simulate();
  display_->SimulatorUpdated(fluid_sim_);
}

void MainWindow::ResetSim() {
  if (sim_thread_ != nullptr && sim_thread_->isRunning())
    return;
  fluid_sim_->InitialiseDensity();
  fluid_sim_->InitialiseVelocity();
  display_->SimulatorUpdated(fluid_sim_);
}

void MainWindow::StartSim() {
  if (sim_thread_ == nullptr) {
    sim_thread_ = new FluidSimulatorThread(fluid_sim_);
  }
  if (sim_thread_->isRunning())
    return;
  connect(sim_thread_,
          &FluidSimulatorThread::SimulationUpdated,
          display_,
          &FluidDisplayRGBWidget::SimulatorUpdated,
          Qt::DirectConnection);
  sim_thread_->start();
}

void MainWindow::StopSim() {
  if (!sim_thread_->isRunning())
    return;
  disconnect(sim_thread_,
             &FluidSimulatorThread::SimulationUpdated,
             display_,
             &FluidDisplayRGBWidget::SimulatorUpdated);

  sim_thread_->requestInterruption();
  sim_thread_->wait();
}

void MainWindow::HandleRightClick(float px, float py) {
  auto x = (uint32_t) std::round(px * fluid_sim_->DimX());
  auto y = (uint32_t) std::round(py * fluid_sim_->DimY());
  auto vx = (px-0.5f) * -12.0f;
  auto vy = (py-0.5f) * -12.0f;
  fluid_sim_->AddSource(x, y, 1.0f, vx, vy);
}