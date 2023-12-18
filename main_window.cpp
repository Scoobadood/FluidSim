#include "main_window.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Initialize the UI components
    auto central_widget = new QWidget(this);
    auto layout = new QHBoxLayout();
    central_widget->setLayout(layout);
    setCentralWidget(central_widget);

    // Graphics
    graphics_view_ = new QGraphicsView(this);
    scene_ = new QGraphicsScene(0, 0, 200, 200, this); // Create a QGraphicsScene
    graphics_view_->setScene(scene_);
    QSizePolicy size_policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    size_policy.setVerticalStretch(1);
    size_policy.setHorizontalStretch(1);
    graphics_view_->setSizePolicy(size_policy);
    layout->addWidget(graphics_view_);

    // Button
    btn_pause_resume_ = new QPushButton("Resume", this);
    size_policy.setHorizontalPolicy(QSizePolicy::Fixed);
    size_policy.setVerticalPolicy(QSizePolicy::Fixed);
    btn_pause_resume_->setSizePolicy(size_policy);
    int preferred_width = btn_pause_resume_->sizeHint().width();
    btn_pause_resume_->setFixedWidth(preferred_width);
    btn_pause_resume_->setText("Pause");
    layout->addWidget(btn_pause_resume_, 0, Qt::AlignTop);

    // Allocate image
    scene_image_ = new QImage(200, 200, QImage::Format_Grayscale8);

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
    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(*scene_image_));
    scene_->clear();
    scene_->addItem(pixmapItem);

    locker.unlock();

    // Perform any other UI updates as needed
}

void MainWindow::UpdateScene(const std::vector<uint8_t> & new_data) {
    // Lock the mutex to protect the data buffer
    QMutexLocker locker(&scene_image_mutex_);

    // Update scene_image_
    uint8_t *pixel_data = reinterpret_cast<uint8_t *>(scene_image_->bits());
    memcpy(pixel_data, new_data.data(), new_data.size());

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

