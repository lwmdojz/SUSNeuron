#include "mainwindow.h"

#include <QtCharts/QChartView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow ControlWindow;

    ControlWindow.show();
    return a.exec();
}
