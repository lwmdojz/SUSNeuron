#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include<QDebug>
#include <QUdpSocket>
#include "plot.h"
#include "ble_finder.h"
#include "debugwindow.h"
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum UdpRecvType {
    UdpSearch,
    UdpOther,
    UdpRead,
    UdpBatt,
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Ui::MainWindow *ui;

private slots:

    // handle UDP data
    void handleCommand();
    void handleElecSig();
    void handleImpedance();

    // Acquisition parameter setting
    void on_pushButton_SetParam_clicked();
    void on_pushButton_calibrate_clicked();
    void on_pushButton_clear_clicked();
    void on_spinBox_SamplingRate_valueChanged(int sampleRate);

    // Acquisition action setting
    void on_pushButton_savePath_clicked();
    void on_pushButton_run_clicked();
    void on_pushButton_getBatt_clicked();
    void on_pushButton_shutdown_clicked();

    // Impedance measurement setting
//    void on_pushButton_channelset_clicked();
//    void on_pushButton_currentset_clicked();
    void on_pushButton_impedancetest_clicked();
//    void on_pushButton_hz_clicked();
//    void on_pushButton_zcount_clicked();

    // Plot Setting
    void on_pushButton_setPlotCh_clicked();

    // Others

    void ActionDebug_triggered();

    void on_pushButton_searchIP_clicked();

    void on_checkBox_DSPOnoff_stateChanged(int arg1);

    void on_checkBox_plot_stateChanged(int arg1);


    void on_pushButton_searchBLE_clicked();

private:

    QUdpSocket  *m_imp;
    QUdpSocket  *m_cmd;
    QUdpSocket  *m_sig;

    QString ip = "192.168.137.24";      // IP address in QString
    bool connect_status = false;

    quint16 sendPort = 2333;
    quint16 shitPort = 2334;
    quint16 recvPort = 2335;

    UdpRecvType recvType;

    QString ImpedanceFileName = "testIMP.csv";
    QString ShitFileName = "testElec.csv";

    Plot *plot;
    BLE_Finder *ble_finder;
    DebugWindow *debugWindow;


    double kFreq[15] = {0.1103, 0.04579, 0.02125, 0.01027,
                        0.005053, 0.002506, 0.001248, 0.0006229,
                        0.0003112, 0.0001555, 0.00007773, 0.00003886,
                        0.00001943, 0.000009714, 0.000004857};


    // Register setting
    void on_pushButton_read_clicked();
    void on_pushButton_write_clicked();

    void Delay_MSec(int msec);

};
#endif // MAINWINDOW_H
