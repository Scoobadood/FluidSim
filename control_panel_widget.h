#ifndef CONTROLPANELWIDGET_H
#define CONTROLPANELWIDGET_H

#include <QPushButton>
#include <QWidget>

class ControlPanelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ControlPanelWidget(QWidget *parent = nullptr);

signals:
    void Start();
    void Stop();
    void Step();
    void Reset();

private:
    void HandleStartStopButton();
    void HandleStepButton();
    void HandleResetButton();
    QPushButton *start_stop_button_;
    QPushButton *step_button_;
    QPushButton *reset_button_;
};

#endif // CONTROLPANELWIDGET_H
