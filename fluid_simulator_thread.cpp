#include "fluid_simulator_thread.h"

#include <QThread>

FluidSimulatorThread::FluidSimulatorThread(GridFluidSimulator *simulator, QObject *parent)
    : QThread{parent}       //
    , simulator_{simulator} //
{
}

void FluidSimulatorThread::run()
{
    while (!isInterruptionRequested()) {
        // Generate data
        simulator_->Simulate();
        emit SimulationUpdated(simulator_);
        QThread::msleep(1);
    }
}
