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
        
    void getPlotData(quint8 plotData[64]);

    void setTimeRange(quint16 range_pt);

    void setPlotChannel(quint16 newChannel);
    quint16 getPlotChannel();

    void Plotting(quint16* pts, quint16 length);

    bool getPlot_status() const;

    void setPlot_status(bool newPlot_status);

    void setPlot_rate(quint32 newPlot_rate);

private:
    QLineSeries *plot_series;

    QValueAxis *plot_axisX;
    QValueAxis *plot_axisY;

    quint32 x_range = 2000;
    quint16 y_range;

    quint64 plot_time;
    quint32 plot_rate;
    quint16 plot_channel = 0;

    bool plot_status;
};

#endif // PLOT_H
