#ifndef FLUID_SIMULATOR_THREAD_H
#define FLUID_SIMULATOR_THREAD_H

#include "grid_fluid_simulator.h"

#include <QThread>

class FluidSimulatorThread : public QThread
{
    Q_OBJECT
public:
    explicit FluidSimulatorThread(GridFluidSimulator *simulator, QObject *parent = nullptr);

signals:
    void SimulationUpdated(GridFluidSimulator *simulator);

protected:
    void run() override;

private:
    GridFluidSimulator *simulator_;
};

#endif // FLUID_SIMULATOR_THREAD_H
