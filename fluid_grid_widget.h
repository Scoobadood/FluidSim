#ifndef FLUID_GRID_WIDGET_H
#define FLUID_GRID_WIDGET_H

#include "fluid_grid.h"

#include <QWidget>
#include <QPixmap>

class FluidGridWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FluidGridWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void updateUI();


private:
    std::shared_ptr<QImage> image_;
    std::shared_ptr<QTimer> timer_;
    std::shared_ptr<FluidGrid> fluid_grid_;
    std::shared_ptr<QThread>fluid_grid_thread_;
};

#endif // FLUID_GRID_WIDGET_H
