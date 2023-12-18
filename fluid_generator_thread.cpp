#include "fluid_generator_thread.h"

FluidGeneratorThread::FluidGeneratorThread(QObject *parent)
    : QThread{parent}
{

}


void FluidGeneratorThread::run() {
    while (!isInterruptionRequested()) {
        // Generate data
        std::vector<uint8_t> newData = UpdateSimulation();
        emit SceneUpdated(newData);
        QThread::msleep(33); // Delay for 30fps
    }
}

std::vector<uint8_t> FluidGeneratorThread::UpdateSimulation() {
    return {};
}
