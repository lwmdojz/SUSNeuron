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

private slots:
    void on_pushButton_SamplingPeriod_clicked();

    void on_pushButton_DSPCutoff_clicked();

    void on_pushButton_DSPOnoff_clicked();

    void on_pushButton_Bandwidth_clicked();


    void on_pushButton_read_clicked();

    void on_pushButton_run_clicked();

    void on_pushButton_savePath_clicked();

    void on_pushButton_getBatt_clicked();

    void on_pushButton_shutdown_clicked();

    void on_pushButton_calibrate_clicked();

    void on_pushButton_channelset_clicked();

    void on_pushButton_currentset_clicked();

    void on_pushButton_impedancetest_clicked();

    void on_pushButton_hz_clicked();

    void on_pushButton_zcount_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket  *mrecv;
    QUdpSocket  *msend;
    udpSave *udp;
    QString ip="192.168.137.102";
    quint16 sendPort=2333;
    UdpRecvType recvTpye;
    QTimer *plotting_timer;
    QString fileName="";
};
#endif // MAINWINDOW_H
