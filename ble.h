#ifndef BLE_H
#define BLE_H

#include <QtCharts>
#include <QtCore>
#include <QtBluetooth>

#include <QBluetoothDeviceDiscoveryAgent>
#include <OBluetoothUuid>
#include <OBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyDescriptor>
#include <QLowEnergyService>

QT_BEGIN_NAMESPACE
class QLineSeries;
class QValueAxis;
QT_END_NAMESPACE

QT_USE_NAMESPACE


class BLE
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

    void Plotting(quint16 pts);

private:
    QLineSeries *plot_series;

    QValueAxis *plot_axisX;
    QValueAxis *plot_axisY;

    quint32 x_range = 2000;
    quint16 y_range;

    quint64 plot_time;
    quint16 plot_channel = 0;
};

#endif // BLE_H
