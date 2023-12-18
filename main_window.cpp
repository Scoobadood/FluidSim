#include "main_window.h"

#include <QGraphicsView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTimer>

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

    fluid_generator_thread_.start();

    // Set timer to update UI.
    auto uiUpdateTimer = new QTimer(this);
    connect(uiUpdateTimer, &QTimer::timeout, this, &MainWindow::UpdateUI);
    uiUpdateTimer->start(33); // 30fps (1000ms / 30fps)
}

void MainWindow::UpdateUI() {
    // Lock the mutex to access the data
    QMutexLocker locker(&scene_image_mutex_);

    // Update the QGraphicsView with the latest data
    // For example:
    // scene_image_ contains the latest data from the generator
    // QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(scene_image_));
    // scene->clear();
    // scene->addItem(pixmapItem);

    locker.unlock();

    // Perform any other UI updates as needed
}

void MainWindow::UpdateScene(const std::vector<uint8_t> & new_data) {
    // Lock the mutex to protect the data buffer
    QMutexLocker locker(&scene_image_mutex_);

    // Update scene_image_
    scene_image_.loadFromData(reinterpret_cast<const uchar*>(new_data.data()), new_data.size());

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

    centralWidget()->layout()->removeWidget(btn_pause_resume_);
    centralWidget()->layout()->removeWidget(graphics_view_);
    delete centralWidget()->layout();
    delete graphics_view_;
    delete btn_pause_resume_;
}

