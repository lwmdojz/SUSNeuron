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
//    QObject::connect(&plotTimer, &QTimer::timeout, this, &Plot::handleTimeout);
//    plotTimer.setInterval(100); // 1000 frame per second

    QPen green(Qt::red);
    green.setWidth(0);

    addAxis(plot_axisX, Qt::AlignBottom);
    addAxis(plot_axisY, Qt::AlignLeft);

    for(int i=1; i<32; i++)
    {
        plot_series[i] = new QLineSeries(this);
//        plot_series[i]->setPen(green);
//        plot_series[i]->useOpenGL();

//        addSeries(plot_series[i]);
//        plot_series[i]->attachAxis(plot_axisX);
//        plot_series[i]->attachAxis(plot_axisY);
    }



    plot_series[0] = new QLineSeries(this);
    plot_series[0]->setPen(green);
    plot_series[0]->useOpenGL();

    addSeries(plot_series[0]);
    plot_series[0]->attachAxis(plot_axisX);
    plot_series[0]->attachAxis(plot_axisY);

    plot_axisX->setRange(0, 100);
    plot_axisY->setRange(0, 65535);

    memset(plot_count,0,sizeof(int)*32);
}

Plot::~Plot()
{

}


void Plot::plotData(QByteArray *datagram)
{
        qDebug() << "plot data get";

    tempStack.append(datagram->data());
    while (tempStack.size() > 64) {
        for(int i=0; i<32; i++) {
            plot_series[i]->append(plot_time, ((quint16)(tempStack[2*i+1])<<8)+tempStack[2*i]);
        }
        tempStack.remove(0, 64);
        plot_time++;

        if (plot_time>1000) {
            for(int i=0; i<32; i++) {
                plot_series[i]->remove(0);
            }
        }
    }
}

//void Plot::plotData(quint8 plotData[64])
//{
//    qDebug() << "plot data get";
//    for (int i=0; i<32; i++)
//    {
//        plot_series[i]->append( plot_time, ((quint16)plotData[2*i+1]<<8) + plotData[2*i] );
//        plot_count[i] += 1;

//        if (plot_count[i] > 1000)    // 10Hz * 60s = 600 pts
//        {
//            qDebug() << "delete data";
//            plot_series[i]->remove(i);        // remove old data
//        }
//    }
//    plot_time++;
//}


//void Plot::handleTimeout()
//{
//    qDebug() << "ask for data signal sent\n";
////    plot_axisX->setRange(plot_time-200, plot_time+200);
//    emit getPlotData();
//}


void Plot::setTimeRange(quint16 range_ms)
{
    plot_axisX->setRange(plot_time - range_ms, plot_time);
}

void Plot::setVoltRange(quint16 range_uv, qint16 center_uv)
{
    plot_axisY->setRange(center_uv - range_uv / 2, center_uv + range_uv / 2);
}

