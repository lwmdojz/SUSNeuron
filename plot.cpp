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
    m_series(0),
    m_axisX(new QValueAxis()),
    m_axisY(new QValueAxis()),
    m_step(0),
    m_x(80),    // initiate point position
    m_y(0)
{
    // Init timer
    QObject::connect(&m_timer, &QTimer::timeout, this, &Plot::handleTimeout);
    m_timer.setInterval(20);   // 20 frame per second

    QPen green(Qt::red);
    green.setWidth(0);

    m_series = new QLineSeries(this);
    m_series->setPen(green);
    m_series->append(m_x, m_y); // plot the init pos

    addSeries(m_series);

    addAxis(m_axisX,Qt::AlignBottom);
    addAxis(m_axisY,Qt::AlignLeft);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);

    m_axisX->setTickCount(21);
    m_axisY->setTickCount(6);
    m_axisX->setRange(0, 100);
    m_axisY->setRange(-10, 10);

    m_timer.start();
}

Plot::~Plot()
{

}

void Plot::handleTimeout()
{
    qreal x = plotArea().width() / (m_axisX->max() - m_axisX->min());
//    qreal y = (m_axisX->max() - m_axisX->min()) / (m_axisX->tickCount()-1);
    m_x += 1;
    m_y = 3*sin(m_x/(2*M_PI*1));
    m_series->append(m_x, m_y);     // append a point (m_x, m_y);
    scroll(x, 0);                   // coordinate move with vector (x,0)
}
