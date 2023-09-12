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
    QPen plot_pen(Qt::red);
    plot_pen.setWidth(0);

    addAxis(plot_axisX, Qt::AlignBottom);
    addAxis(plot_axisY, Qt::AlignLeft);

    amplitude_series = new QLineSeries(this);
    amplitude_series->setPen(plot_pen);
    amplitude_series->useOpenGL();
}

Plot::~Plot()
{

}

void Plot::InitPlotElec()
{
    plot_axisX->setRange(0, x_range);  // fix range
    plot_axisX->setTickCount(5);
    plot_axisX->setMinorTickCount(1);

    plot_axisY->setRange(-6000, 6000);
    plot_axisY->setTickCount(5);

    addSeries(amplitude_series);
    amplitude_series->attachAxis(plot_axisX);
    amplitude_series->attachAxis(plot_axisY);
    amplitude_series->setVisible(true);

    plot_status = true;

    // settings for a better apperence
    setTitle("Dynamic Volt display");
    legend()->hide();
    setAnimationOptions(QChart::NoAnimation);
    setContentsMargins(0, 0, 0, 0);
    setMargins(QMargins(0, 0, 0, 0));
    setBackgroundRoundness(0);
}

void Plot::InitPlotIMP()
{
    plot_axisX->setTickCount(5);
    plot_axisX->setMinorTickCount(1);

    plot_axisY->setTickCount(5);

    addSeries(amplitude_series);
    amplitude_series->attachAxis(plot_axisX);
    amplitude_series->attachAxis(plot_axisY);
    amplitude_series->setVisible(true);

    QPen plot_pen(Qt::blue);
    plot_pen.setWidth(0);
    phase_series = new QLineSeries(this);
    phase_series->setPen(plot_pen);
    phase_series->useOpenGL();

    addSeries(phase_series);
    phase_series->attachAxis(plot_axisX);
    phase_series->attachAxis(plot_axisY);
    phase_series->setVisible(true);

    setTitle("Impedance");

    plot_status = true;

    // settings for a better look
    setTitle("Impedance Curve");
    legend()->hide();
    setAnimationOptions(QChart::NoAnimation);
    setContentsMargins(0, 0, 0, 0);
    setMargins(QMargins(0, 0, 0, 0));
    setBackgroundRoundness(0);
}

void Plot::Plotting(quint16* pts, quint16 length)
{
    removeSeries(amplitude_series);
    removeAxis(plot_axisY);
    for (int i = plot_channel; i < length; i+=32)
    {
        qreal Volt = (pts[i]-32768)*0.195;     // mV
        amplitude_series->append(plot_time, Volt);
        plot_time++;

        if (plot_time > x_range)
        {
            amplitude_series->remove(0);
            setTimeEnd(plot_time);
        }
    }
    addSeries(amplitude_series);
    addAxis(plot_axisY, Qt::AlignLeft);
}

void Plot::Plotting_IMP(float* frequency, float* amplitude, float* phase, quint16 freqCount)
{
    qDebug() << "IMP Plotting";

    removeSeries(amplitude_series);
    removeSeries(phase_series);
    for (int i = 0; i < freqCount; i++)
    {
        amplitude_series->append(frequency[i], amplitude[i]);
        phase_series->append(frequency[i], phase[i]);
    }
    addSeries(amplitude_series);
    addSeries(phase_series);

    amplitude_series->attachAxis(plot_axisX);
    amplitude_series->attachAxis(plot_axisY);
    phase_series->attachAxis(plot_axisX);
    phase_series->attachAxis(plot_axisY);

}

bool Plot::getPlot_status() const
{
    return plot_status;
}

void Plot::setPlot_status(bool newPlot_status)
{
    if (newPlot_status == false)
    {
        removeSeries(amplitude_series);
    }
    plot_status = newPlot_status;
}

void Plot::setTimeRange(quint16 range_start, quint16 range_end)
{
    if (range_start < range_end)
    {
        plot_axisX->setRange(range_start, range_end);
        x_range = range_end - range_start;
    }
}

void Plot::setTimeStart(quint16 range_start)
{
    plot_axisX->setRange(range_start, range_start + x_range);
}

void Plot::setTimeEnd(quint16 range_end)
{
    plot_axisX->setRange(range_end - x_range, range_end);
}

void Plot::setPlotChannel(quint16 newChannel)
{
    plot_channel = newChannel;
    amplitude_series->clear();
    plot_time = 0;
}

quint16 Plot::getPlotChannel()
{
    return plot_channel;
}

void Plot::setPlot_rate(quint32 newPlot_rate)
{
    plot_rate = newPlot_rate;
    x_range = 2 * newPlot_rate;
}
