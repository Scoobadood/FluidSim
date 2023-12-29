#include "control_panel_widget.h"

#include <QHBoxLayout>

ControlPanelWidget::ControlPanelWidget(QWidget *parent)
    : QWidget{parent}
{
    auto layout = new QHBoxLayout(this);
    start_stop_button_ = new QPushButton("Start", this);
    start_stop_button_->setAutoDefault(true);
    connect(start_stop_button_,
            &QPushButton::clicked,
            this,
            &ControlPanelWidget::HandleStartStopButton);
    layout->addWidget(start_stop_button_);

    step_button_ = new QPushButton("Step", this);
    step_button_->setEnabled(true);
    connect(step_button_, &QPushButton::clicked, this, &ControlPanelWidget::HandleStepButton);
    layout->addWidget(step_button_);

    reset_button_ = new QPushButton("Reset", this);
    reset_button_->setEnabled(false);
    connect(reset_button_, &QPushButton::clicked, this, &ControlPanelWidget::HandleResetButton);
    layout->addWidget(reset_button_);
}

void ControlPanelWidget::HandleStartStopButton()
{
    if ("Start" == start_stop_button_->text()) {
        start_stop_button_->setText("Stop");
        step_button_->setEnabled(false);
        reset_button_->setEnabled(false);
        emit Start();
    } else {
        start_stop_button_->setText("Start");
        step_button_->setEnabled(true);
        reset_button_->setEnabled(true);
        emit Stop();
    }
}
void ControlPanelWidget::HandleStepButton()
{
    emit Step();
}
void ControlPanelWidget::HandleResetButton()
{
    start_stop_button_->setText("Start");
    step_button_->setEnabled(true);
    reset_button_->setEnabled(false);
    emit Reset();
}
