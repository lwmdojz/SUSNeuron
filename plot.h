#ifndef PLOT_H
#define PLOT_H

#include <QtCharts>
#include <QtCore>

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

    QTimer plotTimer;

signals:
    void getPlotData();

public slots:
    void plotData(quint8 plotData[64]);

    void setTimeRange(quint16 range_ms);
    void setVoltRange(quint16 range_uv, qint16 center_uv);

    void handleTimeout();

private:
    QLineSeries *plot_series[32];

    QValueAxis *plot_axisX;
    QValueAxis *plot_axisY;

    quint32 plot_time;
    quint16 plot_channel = 0;
    quint8 firstByte[32];
};

#endif // PLOT_H
