#ifndef FLUIDGENERATORTHREAD_H
#define FLUIDGENERATORTHREAD_H

#include "fluid_sim_grid.h"

#include <QThread>

class FluidGeneratorThread : public QThread
{
    Q_OBJECT
public:
    explicit FluidGeneratorThread(uint32_t w, uint32_t h, QObject *parent = nullptr);

signals:
    void SceneUpdated(const std::vector<uint8_t>& data);

protected:
    void run() override;

private:
    FluidGridSimulator *fluid_grid_sim_;
};

#endif // FLUIDGENERATORTHREAD_H
