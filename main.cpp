#include "mainwindow.h"
#include "plot.h"

#include <QtCharts/QChartView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow window;

    Plot *plot = new Plot;
    plot->setTitle("Dynamic Volt display");
    plot->legend()->hide();
    plot->setAnimationOptions(QChart::NoAnimation);

    QChartView chartView(plot);
    chartView.setRenderHint(QPainter::Antialiasing);

    window.setCentralWidget(&chartView);
    window.resize(800,600);
    window.show();
    return a.exec();
}
