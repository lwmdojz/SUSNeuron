#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include<QDebug>
#include <QUdpSocket>
#include "udpsave.h"
#include "plot.h"
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum UdpRecvType {
    UdpOther,
    UdpRead,
    UdpBatt
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Ui::MainWindow *ui;
    Plot *plot;

private slots:

    // Acquisition parameter setting
    void on_pushButton_SampleDSP_clicked();
    void on_pushButton_Bandwidth_clicked();
    void on_pushButton_calibrate_clicked();
    void on_pushButton_clear_clicked();

    // Register setting
    void on_pushButton_read_clicked();
    void on_pushButton_write_clicked();

    // Acquisition action setting
    void on_pushButton_savePath_clicked();
    void on_pushButton_run_clicked();
    void on_pushButton_getBatt_clicked();
    void on_pushButton_shutdown_clicked();

    // Impedance measurement setting
    void on_pushButton_channelset_clicked();
    void on_pushButton_currentset_clicked();
    void on_pushButton_impedancetest_clicked();
    void on_pushButton_hz_clicked();
    void on_pushButton_zcount_clicked();

    // Others
    void handleSend();
    void handleReceive();

private:

    QUdpSocket  *mrecvIMP;
    QUdpSocket  *msend;
    udpSave *udp;
    QString ip;

    quint16 sendPort = 2333;
    quint16 IMPPort = 2335;

    UdpRecvType recvTpye;

    QString IMPDataFileName = "";

};
#endif // MAINWINDOW_H
