#include "mainwindow.h"
#include "plot.h"

#include <QtCharts>
#include <QtWidgets>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow ControlWindow;

    ControlWindow.resize(1080, 700);
    ControlWindow.setWindowTitle(QString("SUSNeuron"));
    ControlWindow.show();
    return a.exec();
}
