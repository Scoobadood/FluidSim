#include "fluid_generator_thread.h"

#include <QThread>

FluidGeneratorThread::FluidGeneratorThread(uint32_t w, uint32_t h, QObject *parent)
    : QThread{parent}
{
    fluid_grid_sim_ = new FluidGridSimulator(w, h);
}

void FluidGeneratorThread::run()
{
    emit SceneUpdated(fluid_grid_sim_->Data());
    while (!isInterruptionRequested()) {
        // Generate data
        fluid_grid_sim_->Simulate();
        emit SceneUpdated(fluid_grid_sim_->Data());
        QThread::msleep(1);
    }
}
