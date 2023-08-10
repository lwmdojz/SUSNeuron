#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include <QWidget>

namespace Ui {
class DebugWindow;
}

class DebugWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DebugWindow(QWidget *parent = nullptr);
    ~DebugWindow();

signals:
    void readRegister(quint8 reg);
    void writeRegister(quint8 reg, quint8 value);


private slots:
    void on_pushButton_read_clicked();

private:
    Ui::DebugWindow *ui;
};

#endif // DEBUGWINDOW_H
