/** Dynamic plot part
 *  Reference: QtCharts example: dynamic spline
 *  Version 1.0: copy
 *
 */

#include "plot.h"
#include <QtCharts>
#include <QtCore>

Plot::Plot(QGraphicsItem *parent, Qt::WindowFlags wFlags):
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    plot_series(0),
    plot_axisX(new QValueAxis()),
    plot_axisY(new QValueAxis()),
    plot_time(0)       // initiate point position
{
    QObject::connect(&plot_timer, &QTimer::timeout, this, &Plot::handleTimeout);
    plot_timer.setInterval(20);   // 50 frame per second

    QPen green(Qt::red);
    green.setWidth(0);

    plot_series = new QLineSeries(this);
    plot_series->setPen(green);
    plot_series->useOpenGL();

    addSeries(plot_series);

    addAxis(plot_axisX,Qt::AlignBottom);
    addAxis(plot_axisY,Qt::AlignLeft);
    plot_series->attachAxis(plot_axisX);
    plot_series->attachAxis(plot_axisY);

    plot_axisX->setRange(0, 1000);
    plot_axisY->setRange(-100, 100);

    voltagePts = plot_series->points();
    plot_timer.start();
}

Plot::~Plot()
{

}



/**
 * @brief Plot::handleTimeout
 *  Add & remove points of the plot
 */
void Plot::handleTimeout()
{
//    plot_series->append(sample_time/plot_timer.interval(), plot_volt[plot_channel][plot_time]);
    sample_time++;

}

/**
 * @brief Plot::loadData
 *  load the data from udp
 * @param channel
 * @param data
 */
void Plot::loadData(quint16 channel, quint16 data)
{

    plot_series->append(plot_time, data);

}

void Plot::nextPt()
{
    plot_time++;
}


void Plot::startPlot()
{
    plot_timer.start();
}

void Plot::pausePlot()
{
    plot_timer.stop();
}

/**
 * @brief Set current plotting channel
 * @param channel
 */
void Plot::setChannel(quint16 channel)
{
    plot_channel = channel;
}

void Plot::setTimeRange(quint16 range_ms)
{
    plot_axisX->setRange(plot_time-range_ms, plot_time);
}

void Plot::setVoltRange(quint16 range_uv, qint16 center_uv)
{
    plot_axisY->setRange(center_uv-range_uv/2, center_uv+range_uv/2);
}
