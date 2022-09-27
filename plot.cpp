/** Dynamic plot part
 *  Reference: QtCharts example: dynamic spline
 *  Version 1.0: copy
 *
 */

#include "plot.h"
#include <QtCharts>
#include <QtCore>

quint8 plotData[64];
quint16 plotSize = 0;

Plot::Plot(QGraphicsItem *parent, Qt::WindowFlags wFlags) : QChart(QChart::ChartTypeCartesian, parent, wFlags),
                                                            plot_axisX(new QValueAxis()),
                                                            plot_axisY(new QValueAxis()),
                                                            plot_time(0) // initiate point position
{
    QObject::connect(&plotTimer, &QTimer::timeout, this, &Plot::handleTimeout);
    plotTimer.setInterval(100); // 10 frame per second

    QPen green(Qt::red);
    green.setWidth(0);

    addAxis(plot_axisX, Qt::AlignBottom);
    addAxis(plot_axisY, Qt::AlignLeft);

    for(int i=0; i<32; i++)
    {
        plot_series[i] = new QLineSeries(this);
        plot_series[i]->setPen(green);
        plot_series[i]->useOpenGL();

        addSeries(plot_series[i]);
        plot_series[i]->attachAxis(plot_axisX);
        plot_series[i]->attachAxis(plot_axisY);
    }

    plot_axisX->setRange(0, 1000);
    plot_axisY->setRange(0, 65535);

    plotSize = 64;
}

Plot::~Plot()
{

}

/**
 * @brief Plot::loadData
 *  load the data from udp
 * @param channel
 * @param data
 */
void Plot::loadData(quint16 channel, quint16 data)
{
    if (!channel)       // temp, plot channel 0 only
    {
        plot_series[channel]->append(plot_time, data);
        if (plot_series[channel]->count() > 600)   // 10Hz * 60s = 600 pts
        {
            plot_series[channel]->remove(0);        // remove old data
        }
    }
}

void Plot::nextPt()
{
    plot_time++;
    qDebug() << plot_time;
    scroll(1,0);
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
    plot_axisX->setRange(plot_time - range_ms, plot_time);
}

void Plot::setVoltRange(quint16 range_uv, qint16 center_uv)
{
    plot_axisY->setRange(center_uv - range_uv / 2, center_uv + range_uv / 2);
}

void Plot::handleTimeout()
{
    if (plotSize == 0)
    {
        for (int i=0; i<32; i++)
        {
            plot_series[i]->append( plot_time, ((quint16)plotData[2*i+1]<<8) + plotData[2*i] );

            if (plot_series[i]->count() > 600)    // 10Hz * 60s = 600 pts
            {
                plot_series[i]->remove(0);        // remove old data
            }
        }
        plotSize = plotSize + 64;
        plot_time++;
    }
}

