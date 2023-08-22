#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include<QDebug>
#include <QUdpSocket>
#include "plot.h"
#include "ble_finder.h"
#include "debugwindow.h"

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
    void on_pushButton_searchIP_clicked();

    // Acquisition parameter setting
    void on_pushButton_setParam_clicked();
    void on_pushButton_calibrate_clicked();
    void on_pushButton_clearCal_clicked();
    void on_checkBox_DSPOnoff_stateChanged(int arg1);
    void on_spinBox_samplingRate_valueChanged(int sampleRate);

    // Acquisition action setting
    void on_pushButton_savePath_clicked();
    void on_pushButton_run_clicked();
    void on_pushButton_getBatt_clicked();
    void on_pushButton_shutdown_clicked();

    // Impedance measurement setting
    void on_pushButton_searchBLE_clicked();
    void on_pushButton_channelSet_clicked();
    void on_pushButton_switchMode_clicked();
    void on_pushButton_applyPara_clicked();
    void on_pushButton_startIMP_clicked();

    // Plot Setting
    void on_pushButton_setPlotCh_clicked();
    void on_checkBox_plot_stateChanged(int arg1);

    // Others
    void ActionDebug_triggered();
    void BLE_DeviceConnected();

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

    uint8_t* StringToByteArray(QString string);
    uint8_t* CmdFloatToByteArray(QString string, float value);
    
};
#endif // MAINWINDOW_H
