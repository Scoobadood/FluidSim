#ifndef FLUIDGENERATORTHREAD_H
#define FLUIDGENERATORTHREAD_H

#include <QThread>

class FluidGeneratorThread : public QThread
{
    Q_OBJECT
public:
    explicit FluidGeneratorThread(QObject *parent = nullptr);

signals:
    void SceneUpdated(const std::vector<uint8_t>& data);

protected:
    void run() override;

private:
    std::vector<uint8_t> UpdateSimulation();
};

#endif // FLUIDGENERATORTHREAD_H
