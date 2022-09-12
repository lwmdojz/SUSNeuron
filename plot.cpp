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
    plot_time(80)       // initiate point position
{
    // Init timer
    QObject::connect(&plot_timer, &QTimer::timeout, this, &Plot::handleTimeout);
    plot_timer.setInterval(20);   // 50 frame per second

    QPen green(Qt::red);
    green.setWidth(0);

    plot_series = new QLineSeries(this);
    plot_series->setPen(green);

    addSeries(plot_series);

    addAxis(plot_axisX,Qt::AlignBottom);
    addAxis(plot_axisY,Qt::AlignLeft);
    plot_series->attachAxis(plot_axisX);
    plot_series->attachAxis(plot_axisY);

    plot_axisX->setTickCount(8);
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
//    qreal x = plotArea().width() / (plot_axisX->max() - plot_axisX->min());
    plot_series->append(sample_time/plot_timer.interval(), plot_volt[plot_channel][plot_time]);
    scroll(sample_time*plot_timer.interval(), 0);                   // coordinate move with vector (x,0)
    sample_time++;
}

void Plot::loadData(quint16 channel, quint16 data)
{
    plot_volt[channel][plot_time%60000] = data;
}

void Plot::setChannel(quint16 channel)
{
    plot_channel = channel;
}

void Plot::nextPt()
{
    plot_time++;
}

void Plot::setTimeRange(quint16 range_ms)
{
    plot_axisX->setRange(plot_time-range_ms, plot_time);
}

void Plot::setVoltRange(quint16 range_uv, qint16 center_uv)
{
    plot_axisY->setRange(center_uv-range_uv/2, center_uv+range_uv/2);
}
