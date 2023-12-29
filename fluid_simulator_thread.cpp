#include "fluid_simulator_thread.h"

#include <QThread>

FluidSimulatorThread::FluidSimulatorThread(FluidSimulator2D *simulator, QObject *parent)
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
