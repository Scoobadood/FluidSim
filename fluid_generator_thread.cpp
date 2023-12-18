#include "fluid_generator_thread.h"

#include <QThread>

FluidGeneratorThread::FluidGeneratorThread(QObject *parent)
    : QThread{parent}
{
    fluid_grid_sim_ = new FluidGridSimulator(200, 200);
}


void FluidGeneratorThread::run() {
    while (!isInterruptionRequested()) {
        // Generate data
        fluid_grid_sim_->Simulate();
        emit SceneUpdated(fluid_grid_sim_->Data());
        QThread::msleep(5);
    }
}
