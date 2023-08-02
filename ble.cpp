/** Dynamic plot part
 *  Reference: QtCharts example: dynamic spline
 *  Version 1.0: copy
 *
 */

#include "ble.h"

#include <QLowEnergyCharacteristic>#include <QtCore>

Plot::Plot
{
    // 创建设备发现对象
    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    m_btLocalDevice = new QBluetoothLocalDevice();                                 // 创建本地设备对象
    m_btSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);      // 蓝牙套接字
    QBluetoothAddress adapterAddress = m_btLocalDevice->address();                 // 使用默认蓝牙适配器
    m_serviceDiscoveryAgent = new QBluetoothServiceDiscoveryAgent(adapterAddress); // 创建服务发现对象
    // 连接信号与槽(deviceDiscoveryAgent是代码中自己声明的对象,ui中没有，无法自动生成槽函数;需要自己写槽函数，自己连接)

    /*-------------------- 初始化设备列表与服务列表 -----------------*/
    QListWidgetItem *item = new QListWidgetItem();    // 设置设备列表

    BluetoothDeviceCell *btDevCell = new BluetoothDeviceCell();    // 创建自定义窗口,放入到listwidget中

    item->setSizeHint(QSize(ui->deviceListWidget->width(), btDevCell->height()));    // 设置item的高

    // 设置label显示
    // 第一个加进去的item在最上面，相当于标题
    btDevCell->m_btName->setText("名称");
    btDevCell->m_btAddr->setText("地址");
    btDevCell->m_btRssi->setText("信号强度");

    ui->deviceListWidget->addItem(item);    // 将item加入到listwidget中

    ui->deviceListWidget->setItemWidget(item, btDevCell);    // 设置item的窗口为自定义的窗口


    QListWidgetItem *items = new QListWidgetItem();    // 设置服务列表的标题

    BluetoothDeviceCell *btDevCells = new BluetoothDeviceCell();    // 创建自定义窗口，放入到listwidget中

    items->setSizeHint(QSize(ui->serviceListWidget->width(), btDevCells->height()));    // 设定item的尺寸

    // 设置label的显示
    // 第一个加进去的item在最上面，相当于标题
    btDevCells->m_btName->setText("名称");
    btDevCells->m_btAddr->setText("服务Uuid");
    btDevCells->m_btRssi->setText("空");
    ui->serviceListWidget->addItem(items);    // 将item加入到listwidget中

    ui->serviceListWidget->setItemWidget(items, btDevCells);    // 设置item的窗口为自定义的窗口

    /*------------- m_deviceDiscoveryAgent设备搜索对象的信号槽 -------------*/
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothConnect::deviceDiscoveredSlot);                 // 发现设备
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BluetoothConnect::deviceSearchFinishedSlot);                     // 搜索完毕
    void (QBluetoothDeviceDiscoveryAgent::*deviceSearchErrorOccurred)(QBluetoothDeviceDiscoveryAgent::Error) = &QBluetoothDeviceDiscoveryAgent::error; // 有重载
    connect(m_deviceDiscoveryAgent, deviceSearchErrorOccurred, this, &BluetoothConnect::deviceSearchErrorOccurredSlot);                                // 设备搜索发生错误

    /*------------- m_btSocket蓝牙套接字的信号槽 ----------------*/
    void (QBluetoothSocket::*socketErrorOccurred)(QBluetoothSocket::SocketError) = &QBluetoothSocket::error;
    connect(m_btSocket, socketErrorOccurred, this, &BluetoothConnect::socketErrorOccurredSlot);      // 错误处理槽函数
    connect(m_btSocket, &QBluetoothSocket::connected, this, &BluetoothConnect::socketConnectedSlot); // 连接成功

    /*------------- m_serviceDiscoveryAgent服务发现对象的信号槽 ---------------*/
    connect(m_serviceDiscoveryAgent, &QBluetoothServiceDiscoveryAgent::serviceDiscovered, this, &BluetoothConnect::serviceDiscoveredSlot); // 发现一个服务
    connect(m_serviceDiscoveryAgent, &QBluetoothServiceDiscoveryAgent::finished, this, &BluetoothConnect::serviceSearchFinishedSlot);      // 服务搜索完毕
}

Plot::~Plot()
{
}

void Plot::Plotting(quint16 pts)
{
    qreal Volt = pts;
    plot_series->append(plot_time, Volt);

    if (plot_time++ == x_range)
    {
        plot_series->clear();
        plot_time = 0;
    }
}

void Plot::setTimeRange(quint16 range_pt)
{
    x_range = range_pt;
    plot_axisX->setRange(0, range_pt);
}

void Plot::setPlotChannel(quint16 newChannel)
{
    plot_channel = newChannel;
    plot_series->clear();
    plot_time = 0;
}

quint16 Plot::getPlotChannel()
{
    return plot_channel;
}
