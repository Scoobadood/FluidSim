#ifndef CONTROL_PANEL_WIDGET_H
#define CONTROL_PANEL_WIDGET_H

#include <QCheckBox>
#include <QPushButton>
#include <QWidget>

class ControlPanelWidget : public QWidget {
Q_OBJECT

public:
  explicit ControlPanelWidget(QWidget *parent = nullptr);

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
    void Start();

    void Stop();

    void Step();

    void Reset();

    void ShowDensity(bool);

    void ShowVelocity(bool);
#pragma clang diagnostic pop

private:
  void HandleStartStopButton();

  void HandleStepButton();

  void HandleResetButton();

  void HandleDensityCheckbox(bool checked);

  void HandleVelocityCheckbox(bool checked);

  QPushButton *start_stop_button_;
  QPushButton *step_button_;
  QPushButton *reset_button_;
  QCheckBox *density_checkbox_;
  QCheckBox *velocity_checkbox_;
};

#endif // CONTROL_PANEL_WIDGET_H
