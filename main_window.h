#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "fluid_generator_thread.h"

#include <QGraphicsView>
#include <QMainWindow>
#include <QMutex>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void ToggleSceneGeneration();
    void UpdateScene(const std::vector<uint8_t> & new_data);
    void UpdateUI();

private:
    QGraphicsView* graphics_view_;
    QGraphicsScene *scene_;
    QPushButton* btn_pause_resume_;
    QImage *scene_image_;
    QMutex scene_image_mutex_;
    FluidGeneratorThread fluid_generator_thread_;
};


#endif // MAIN_WINDOW_H
