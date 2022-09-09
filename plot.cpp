/** Dynamic plot part
 *  Reference: QtCharts example: dynamic spline
 *  Version 1.0: copy
 *
 */

#include "plot.h"
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QRandomGenerator>
#include <QtCore/QDebug>

Plot::Plot(QGraphicsItem *parent, Qt::WindowFlags wFlags):
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    plot_series(0),
    plot_axisX(new QValueAxis()),
    plot_axisY(new QValueAxis()),
    plot_step(0),
    plot_x(80),    // initiate point position
    plot_y(0)
{
    // Init timer
    QObject::connect(&plot_timer, &QTimer::timeout, this, &Plot::handleTimeout);
    plot_timer.setInterval(20);   // 20 frame per second

    QPen green(Qt::red);
    green.setWidth(0);

    plot_series = new QLineSeries(this);
    plot_series->setPen(green);
    plot_series->append(plot_x, plot_y); // plot the init pos

    addSeries(plot_series);

    addAxis(plot_axisX,Qt::AlignBottom);
    addAxis(plot_axisY,Qt::AlignLeft);
    plot_series->attachAxis(plot_axisX);
    plot_series->attachAxis(plot_axisY);

    plot_axisX->setTickCount(21);
    plot_axisY->setTickCount(6);
    plot_axisX->setRange(0, 100);
    plot_axisY->setRange(-10, 10);

    plot_timer.start();
}

Plot::~Plot()
{

}

void Plot::handleTimeout()
{
    qreal x = plotArea().width() / (plot_axisX->max() - plot_axisX->min());
    plot_x += 1;
    plot_y = 3*sin(plot_x/(2*PI*1));
    plot_series->append(plot_x, plot_y);     // append a point (plot_x, plot_y);
    scroll(x, 0);                   // coordinate move with vector (x,0)
}


