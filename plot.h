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

    void InitPlotElec(void);
    void Plotting(quint16* pts, quint16 length);
    void setPlotChannel(quint16 newChannel);
    quint16 getPlotChannel();

    void InitPlotIMP(void);

    bool getPlot_status() const;
    void setPlot_status(bool newPlot_status);

    void setTimeRange(quint16 range_start, quint16 range_end);
    void setTimeStart(quint16 range_start);
    void setTimeEnd(quint16 range_end);

    void resetAxis(void);

    void clear(void);


    void setPlot_rate(quint32 newPlot_rate);

public slots:
    void Plotting_IMP(float* frequency, float* amplitude, float* phase, quint16 freqCount);


private:
    QLineSeries *amplitude_series;
    QLineSeries *phase_series;

    QValueAxis *plot_axisX;
    QValueAxis *plot_axisY;
    QValueAxis *plot_axisZ;

    quint32 x_range = 2000;
    quint16 y_range;

    float imp_max = 0, imp_min = 0;

    quint64 plot_time;
    quint32 plot_rate;
    quint16 plot_channel = 0;

    bool plot_status;
};

#endif // PLOT_H
