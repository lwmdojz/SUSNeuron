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
    QObject::connect(&plotTimer, &QTimer::timeout, this, &Plot::handleTimeout);
    plotTimer.setInterval(25); // 50 frame per second

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
        plot_series[i]->setVisible(false);
    }
    plot_series[0]->setVisible(true);

    plot_axisX->setRange(-20, 220);
    plot_axisY->setRange(0, 65535);

}

Plot::~Plot()
{

}


void Plot::plotData(quint8 plotData[64])
{
//    qDebug() << "plot data get";

    if (plot_time > 200)    // 10Hz * 60s = 600 pts
    {
        for (int i=0; i<32; i++)
        {
            plot_series[i]->remove(0);
            plot_series[i]->append( plot_time, ((quint16)(plotData[2*i+1])<<8)+(quint16)plotData[2*i] );
        }
//        qDebug() << "delete data";
        scroll(this->plotArea().width() / 240, 0);
    }
    else
    {
        for (int i=0; i<32; i++)
        {
            plot_series[i]->append( plot_time, ((quint16)(plotData[2*i+1])<<8)+(quint16)plotData[2*i] );
        }
    }
    plot_time++;
}

void Plot::handleTimeout()
{
//    qDebug() << "ask for data signal sent\n";
//    plot_axisX->setRange(plot_time-200, plot_time+200);
    emit getPlotData();
}


void Plot::setTimeRange(quint16 range_ms)
{
    plot_axisX->setRange(plot_time - range_ms, plot_time);
}

void Plot::setVoltRange(quint16 range_uv, qint16 center_uv)
{
    plot_axisY->setRange(center_uv - range_uv / 2, center_uv + range_uv / 2);
}

