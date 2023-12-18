#include "main_window.h"

#include <QGraphicsView>
#include <QPushButton>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Initialize the UI components
    graphics_view_ = new QGraphicsView(this);
    btn_pause_resume_ = new QPushButton("Pause", this);

    auto centralWidget = new QWidget(this);

    auto layout = new QHBoxLayout();
    layout->addWidget(graphics_view_);
    layout->addWidget(btn_pause_resume_);

    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Connect signals and slots for scene updates
    connect(&fluid_generator_thread_, &FluidGeneratorThread::SceneUpdated, this, &MainWindow::UpdateScene);
    connect(btn_pause_resume_, &QPushButton::clicked, this, &MainWindow::ToggleSceneGeneration);

}

void MainWindow::UpdateScene(const std::vector<uint8_t> & new_data) {
    // Lock the mutex to protect the data buffer
    QMutexLocker locker(&scene_image_mutex_);

    // Update scene_image_

    locker.unlock();

    // Update the QGraphicsView or perform any other processing with the data
    // ...
}

void MainWindow::ToggleSceneGeneration() {
    if (fluid_generator_thread_.isRunning()) {
        // Pause data generation
        fluid_generator_thread_.requestInterruption();
        fluid_generator_thread_.wait();
        btn_pause_resume_->setText("Resume");
    } else {
        // Resume data generation
        fluid_generator_thread_.start();
        btn_pause_resume_->setText("Pause");
    }
}

MainWindow::~MainWindow()
{
    // Stop and clean up the data generation thread
    if (fluid_generator_thread_.isRunning()) {
        fluid_generator_thread_.requestInterruption();
        fluid_generator_thread_.wait();
    }
    disconnect(btn_pause_resume_);
    disconnect(&fluid_generator_thread_);

    centralWidget()->layout()->removeWidget(btn_pause_resume_);
    centralWidget()->layout()->removeWidget(graphics_view_);
    delete graphics_view_;
    delete btn_pause_resume_;
}

