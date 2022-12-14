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
    void loadData(quint16 channel, quint16 data);
    void setChannel(quint16 channel);
    void nextPt();
    void setTimeRange(quint16 range_ms);
    void setVoltRange(quint16 range_uv, qint16 center_uv);

private:
    QTimer plot_timer;
    QLineSeries *plot_series;

    QValueAxis *plot_axisX;
    QValueAxis *plot_axisY;

    quint32 plot_time, sample_time;
    qreal plot_volt[32][65535];
    quint16 plot_channel = 0;
};

#endif // PLOT_H
