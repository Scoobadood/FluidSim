#ifndef FLUID_SIMULATOR_THREAD_H
#define FLUID_SIMULATOR_THREAD_H

#include "fluid_sim_grid.h"

#include <QThread>

class FluidSimulatorThread : public QThread
{
    Q_OBJECT
public:
    explicit FluidSimulatorThread(FluidGridSimulator *simulator, QObject *parent = nullptr);

signals:
    void SimulationUpdated(FluidGridSimulator *simulator);

protected:
    void run() override;

private:
    FluidGridSimulator *simulator_;
};

#endif // FLUID_SIMULATOR_THREAD_H
