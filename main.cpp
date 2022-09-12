#include "mainwindow.h"
#include "plot.h"

#include <QtCharts/QChartView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow ControlWindow;

    ControlWindow.plot = new Plot;
    ControlWindow.plot->setTitle("Dynamic Volt display");
    ControlWindow.plot->legend()->hide();
    ControlWindow.plot->setAnimationOptions(QChart::NoAnimation);

    QChartView chartView(ControlWindow.plot);
    chartView.setRenderHint(QPainter::Antialiasing);

    ControlWindow.PlotWindow.setCentralWidget(&chartView);
    ControlWindow.PlotWindow.resize(800,600);
    ControlWindow.show();
    return a.exec();
}
