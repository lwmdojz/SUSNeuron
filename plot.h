#ifndef PLOT_H
#define PLOT_H

#include <QtCharts/QChart>
#include <QtCore/QTimer>

QT_BEGIN_NAMESPACE
class QLineSeries;
class QValueAxis;
QT_END_NAMESPACE

QT_USE_NAMESPACE

#define PI 3.14159265358979323846 /* pi */

class Plot : public QChart
{
    Q_OBJECT
public:
    Plot(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = {});
    virtual ~Plot();

public slots:
    void handleTimeout();
    void setDataPoint(quint32 data);
    //    void setTimeRange();
    //    void setVoltRange();

private:
    QTimer plot_timer;
    QLineSeries *plot_series;
    QStringList plot_titles;
    QValueAxis *plot_axisX;
    QValueAxis *plot_axisY;
    qreal plot_step;
    qreal plot_x[2048];
    qreal plot_y[2048];

    quint32 queueHead;
};

#endif // PLOT_H
