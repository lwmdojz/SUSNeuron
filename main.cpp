#include "mainwindow.h"
#include "plot.h"

#include <QtCharts/QChartView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow PlotWindow;
    MainWindow ControlWindow;

    Plot *plot = new Plot;
    plot->setTitle("Dynamic Volt display");
    plot->legend()->hide();
    plot->setAnimationOptions(QChart::NoAnimation);

    QChartView chartView(plot);
    chartView.setRenderHint(QPainter::Antialiasing);

    PlotWindow.setCentralWidget(&chartView);
    PlotWindow.resize(800,600);
    PlotWindow.show();
    ControlWindow.show();
    return a.exec();
}
