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
                                                            plot_axisZ(new QValueAxis()),
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
    plot_axisX->setTitleText("采样点（pts）");

    plot_axisY->setRange(-6000, 6000);
    plot_axisY->setTickCount(5);
    plot_axisY->setTitleText("电压（uV）");

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
    addAxis(plot_axisZ, Qt::AlignRight);

    plot_axisX->setRange(0, 10000);
    plot_axisX->setTickCount(5);
    plot_axisX->setMinorTickCount(1);
    plot_axisX->setTitleText("频率（Hz）");

    plot_axisY->setRange(800, 1200);
    plot_axisY->setTickCount(5);
    plot_axisY->setTitleText("幅值（Ohm）");

    plot_axisZ->setRange(-10, 10);
    plot_axisZ->setTickCount(5);
    plot_axisZ->setTitleText("相位（°）");

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
    phase_series->attachAxis(plot_axisZ);
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

    for (int i = plot_channel+1; i < length; i+=32)
    {
        amplitude_series->append(plot_time, (pts[i]-32768)*0.195);
        plot_time++;

        if (plot_time == x_range)
        {
            amplitude_series->clear();
            plot_time = 0;
        }
    }

    addSeries(amplitude_series);

    amplitude_series->attachAxis(plot_axisX);
    amplitude_series->attachAxis(plot_axisY);
}

void Plot::Plotting_IMP(float* frequency, float* amplitude, float* phase, quint16 freqCount)
{
    qDebug() << "IMP Plotting";

    amplitude_series->clear();
    phase_series->clear();

    removeSeries(amplitude_series);
    removeSeries(phase_series);

    imp_min = amplitude[1];
    imp_max = amplitude[1];

    for (int i = 0; i < freqCount; i++)
    {
        amplitude_series->append(frequency[i], amplitude[i]);
        if (amplitude[i] > imp_max)
        {
            imp_max = amplitude[i];
        }
        else if (amplitude[i] < imp_min)
        {
            imp_min = amplitude[i];
        }
        phase_series->append(frequency[i], phase[i]);
    }
    plot_axisX->setRange(frequency[0], frequency[freqCount-1]);
    plot_axisY->setRange(imp_min*0.97, imp_max*1.03);

    addSeries(amplitude_series);
    addSeries(phase_series);

    amplitude_series->attachAxis(plot_axisX);
    amplitude_series->attachAxis(plot_axisY);
    phase_series->attachAxis(plot_axisX);
    phase_series->attachAxis(plot_axisZ);
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

void Plot::resetAxis()
{
    plot_axisY->setRange(-6000, 6000);
    plot_axisX->setRange(0, x_range);  // fix range
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

