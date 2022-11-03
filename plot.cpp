/** Dynamic plot part
 *  Reference: QtCharts example: dynamic spline
 *  Version 1.0: copy
 *
 */

#include "plot.h"
#include <QtCharts>
#include <QtCore>

Plot::Plot(QGraphicsItem *parent, Qt::WindowFlags wFlags) : QChart(QChart::ChartTypeCartesian, parent, wFlags),
                                                            plot_axisX(new QValueAxis()),
                                                            plot_axisY(new QValueAxis()),
                                                            plot_time(0) // initiate point position
{
    QPen green(Qt::red);
    green.setWidth(0);

    addAxis(plot_axisX, Qt::AlignBottom);
    addAxis(plot_axisY, Qt::AlignLeft);

    plot_series = new QLineSeries(this);
    plot_series->setPen(green);
    plot_series->useOpenGL();

    addSeries(plot_series);
    plot_series->attachAxis(plot_axisX);
    plot_series->attachAxis(plot_axisY);
    plot_series->setVisible(true);

    setTitle("Volt");

    plot_axisX->setRange(0, x_range);  // fix range
    plot_axisX->setTickCount(5);
    plot_axisX->setMinorTickCount(1);

    plot_axisY->setRange(-5, 5);
    plot_axisY->setTickCount(5);

    // settings for a better look
    setTitle("Dynamic Volt display");
    legend()->hide();
    setAnimationOptions(QChart::NoAnimation);
    setContentsMargins(0, 0, 0, 0);
    setMargins(QMargins(0, 0, 0, 0));
    setBackgroundRoundness(0);

}

Plot::~Plot()
{

}

void Plot::Plotting(quint16 pts)
{
    qreal Volt = (float)pts*5/65535-5;
    plot_series->append(plot_time, Volt);

    if (plot_time++ == x_range)
    {
        plot_series->clear();
        plot_time = 0;
    }

}

void Plot::setTimeRange(quint16 range_pt)
{
    x_range = range_pt;
    plot_axisX->setRange(0, range_pt);
}

void Plot::setPlotChannel(quint16 newChannel)
{
    plot_channel = newChannel;
    plot_series->clear();
    plot_time = 0;
}

quint16 Plot::getPlotChannel()
{
    return plot_channel;
}


