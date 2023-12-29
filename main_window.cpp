#include "main_window.h"
#include "control_panel_widget.h"
#include "fluid_display_widget.h"

#include <QDockWidget>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}  //
    , sim_thread_{nullptr} //
{
    auto control_panel = new ControlPanelWidget(this);

    // Create a QDockWidget
    auto dock = new QDockWidget("Controls", this);
    dock->setWidget(control_panel);
    addDockWidget(Qt::TopDockWidgetArea, dock);
    
    fluid_sim_ = new GridFluidSimulator(200, 200);

    // Add some central content to the main window
    display_ = new FluidDisplayWidget(this);
    setCentralWidget(display_);

    connect(control_panel, &ControlPanelWidget::Start, this, &MainWindow::StartSim);
    connect(control_panel, &ControlPanelWidget::Stop, this, &MainWindow::StopSim);
    connect(control_panel, &ControlPanelWidget::Reset, this, &MainWindow::ResetSim);
    connect(control_panel, &ControlPanelWidget::Step, this, &MainWindow::StepSim);
}

void MainWindow::StepSim()
{
    fluid_sim_->Simulate();
    display_->SimulatorUpdated(fluid_sim_);
}

void MainWindow::ResetSim()
{
    if (sim_thread_ != nullptr && sim_thread_->isRunning())
        return;
    fluid_sim_->InitialiseDensity();
    display_->SimulatorUpdated(fluid_sim_);
}

void MainWindow::StartSim()
{
    if (sim_thread_ == nullptr) {
        sim_thread_ = new FluidSimulatorThread(fluid_sim_);
    }
    if (sim_thread_->isRunning())
        return;
    connect(sim_thread_,
            &FluidSimulatorThread::SimulationUpdated,
            display_,
            &FluidDisplayWidget::SimulatorUpdated);
    sim_thread_->start();
}

void MainWindow::StopSim()
{
    if (!sim_thread_->isRunning())
        return;
    disconnect(sim_thread_,
               &FluidSimulatorThread::SimulationUpdated,
               display_,
               &FluidDisplayWidget::SimulatorUpdated);

    sim_thread_->requestInterruption();
    sim_thread_->wait();
}
