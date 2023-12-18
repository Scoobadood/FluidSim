#include "fluid_generator_thread.h"

#include <QThread>

FluidGeneratorThread::FluidGeneratorThread(QObject *parent)
    : QThread{parent}
{

}


void FluidGeneratorThread::run() {
    while (!isInterruptionRequested()) {
        // Generate data
        std::vector<uint8_t> newData = UpdateSimulation();
        emit SceneUpdated(newData);
        QThread::msleep(10);
    }
}

std::vector<uint8_t> FluidGeneratorThread::UpdateSimulation() {
    return {};
}
