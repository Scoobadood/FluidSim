#ifndef FLUID_SIMULATOR_THREAD_H
#define FLUID_SIMULATOR_THREAD_H

#include "fluid_simulator_2d.h"

#include <QThread>

class FluidSimulatorThread : public QThread
{
    Q_OBJECT
public:
    explicit FluidSimulatorThread(FluidSimulator2D *simulator, QObject *parent = nullptr);

signals:
    void SimulationUpdated(FluidSimulator2D *simulator);

protected:
    void run() override;

private:
    FluidSimulator2D *simulator_;
};

#endif // FLUID_SIMULATOR_THREAD_H
