#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCharts>
#include <QtWidgets>
#include <QFileDialog>
#include <QSettings>
#include <QtCore>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // read config.ini to get ip address
    QFileInfo iniFileInfo("config.ini"); // init, read .Ini file
    if (iniFileInfo.isFile())
    {
        QSettings *configIniRead = new QSettings("config.ini", QSettings::IniFormat);
        ip = configIniRead->value("ip").toString();
        qDebug() << configIniRead->value("ip").toString();
    }
    // if no such file, create a new one

    // UDP init
    m_cmd = new QUdpSocket;
    m_cmd->bind(sendPort); // for normal commend, 2333

    m_sig = new QUdpSocket;
    m_sig->bind(shitPort); // for shit data, 2334

    m_imp = new QUdpSocket;
    m_imp->bind(recvPort); // for impedance test, 2335

    recvType = UdpOther;

    // signal-slot, read data - handle
    connect(m_cmd, &QUdpSocket::readyRead, this, &MainWindow::handleCommand);
    connect(m_sig, &QUdpSocket::readyRead, this, &MainWindow::handleElecSig);
    connect(m_imp, &QUdpSocket::readyRead, this, &MainWindow::handleImpedance);

    // set default choice of comvo boxes
    ui->comboBox_upperCutoff->setCurrentIndex(3);
    ui->comboBox_lowerCutoff->setCurrentIndex(24);

    // DSP cutoff frequency calculate
    for (int i = 1; i < 16; i++)
    {
        ui->comboBox_DSP->setItemText(i, QString::number(ui->spinBox_samplingRate->value() * kFreq[i - 1], 'g', 10) + " Hz");
    }
    ui->comboBox_DSP->setCurrentIndex(6);

    // signal-slot, Debug window
    connect(ui->actionDebug, SIGNAL(triggered()), this, SLOT(ActionDebug_triggered()));

    // init BLE object
    ble_finder = new BLE_Finder;
    connect(ble_finder, SIGNAL(deviceConnected()), this, SLOT(BLE_DeviceConnected()));

    // init Plot object
    elecplot = new Plot;
    elecplot->InitPlotElec();
    ui->graphicsView->setChart(elecplot);
    ui->graphicsView->setRubberBand(QChartView::RectangleRubberBand);

    impplot = new Plot;
    impplot->InitPlotIMP();
    ui->graphicsView_IMP->setChart(impplot);
    ui->graphicsView_IMP->setRubberBand(QChartView::RectangleRubberBand);
    connect(ble_finder, SIGNAL(dataReceived(float*,float*,float*,quint16)), impplot, SLOT(Plotting_IMP(float*,float*,float*,quint16)));
}

MainWindow::~MainWindow()
{
    delete ui;
    close();
    ble_finder->close();
    debugWindow->close();
}

/********** Udp handle function  **********/

/**
 * @brief handle send command feedback
 */
void MainWindow::handleCommand()
{
    QHostAddress cli_addr; // client address
    quint16 port;          // client port

    // init buffer
    char buf[1024] = {0};
    qint64 len = m_cmd->readDatagram(buf, sizeof(buf), &cli_addr, &port);

    if (len > 0)
    {
        // print debug data
        qDebug() << QString("[%1:%2] %3").arg(cli_addr.toString()).arg(port).arg(buf);

        // read register feedback
        if (recvType == UdpRead)
        {
            QString reg = "0x" + QString::number((quint8)buf[0], 16);
            qDebug() << reg;
        }
        // Battery voltage feedback
        else if (recvType == UdpBatt)
        {
            QString bat = QString::number(((quint8)buf[0] + (((quint16)buf[1]) << 8)) * 2) + " mV";
            qDebug() << bat;
            ui->label_batt->setText(bat);
        }
        // Udp Search result
        else if (recvType == UdpSearch && buf[0] == 'a')
        {
            connect_status = true;
            ip = cli_addr.toString();
            ui->label_status->setText("设备状态：已连接");
            qDebug() << ip;
        }
        recvType = UdpOther;

     }
}

/**
 * @brief Handle Impedance test data
 */
void MainWindow::handleImpedance()
{
    QHostAddress cli_addr; // Client address
    quint16 port;          // Client port

    // buffer init
    char buf[2048] = {0};
    qint64 len = m_imp->readDatagram(buf, sizeof(buf), &cli_addr, &port);

    qDebug() << "Impedance test" << len;

    if (len > 0) // get feedback
    {
        if (ImpedanceFileName != "")
        {
            QFile file(ImpedanceFileName);
            if (!(file.open(QFile::WriteOnly | QIODevice::Append)))
            {
                file.close();
            }
            else
            {
                QTextStream out(&file);
                for (int i = 0; i < len / 2; i++)
                {
                    out << ((quint16)((quint8)buf[2 * i + 1]) << 8) + (quint8)buf[2 * i] << "\n";
                    qDebug() << (quint8)buf[2 * i + 1] << (quint8)buf[2 * i];
                }
                file.close();
            }
        }
    }
}

/**
 * @brief handle Electrophysiology data
 */
void MainWindow::handleElecSig()
{
    QHostAddress cli_addr; // Client address
    quint16 port;          // Client port

    // buffer init
    char buf[2084] = {0};
    qint32 len = m_sig->readDatagram(buf, sizeof(buf), &cli_addr, &port);

    if (len > 0) // get feedback
    {
        if (ShitFileName != "")
        {
            QFile file(ShitFileName);
            if (!(file.open(QFile::WriteOnly | QIODevice::Append)))
            {
                file.close();
            }
            else
            {
                QTextStream out(&file);
                quint16 temp[1024];
                for (int i = 1; i < len / 2; i++)
                {
                    temp[i] = ((quint16)((quint8)buf[2 * i + 1]) << 8) + (quint8)buf[2 * i];
                    out << temp[i] << ",";

                    if (i % 32 == 0 && i != 0)
                    {
                        out << "\n";
                    }
                }

                if (elecplot->getPlot_status() == true)
                {
                    elecplot->Plotting(temp, len / 2);
                }
                file.close();
            }
        }
    }
}

void MainWindow::on_pushButton_searchIP_clicked()
{
    // To Do: IP Search
    QString ip_temp = "192.168.137.0"; // IP address in QString

    for (quint16 i = 0; i < 256; i++)
    {
        recvType = UdpSearch;
        ip_temp = "192.168.137." + QString::number(i);
        QString ParamCmd = "m" + QString::number(ui->spinBox_samplingRate->value(), 10);
        m_cmd->writeDatagram(ParamCmd.toUtf8(), QHostAddress(ip_temp), sendPort);

        Delay_MSec(10);

        if (connect_status)
        {
            break;
        }
    }
}

/********** Acquisition parameter setting **********/

void MainWindow::on_pushButton_setParam_clicked()
{
    QString ParamCmd = "m" + QString::number(ui->spinBox_samplingRate->value(), 10);
    m_cmd->writeDatagram(ParamCmd.toUtf8(), QHostAddress(ip), sendPort);

    ParamCmd = "u" + QString::number(ui->comboBox_DSP->currentIndex(), 10);
    m_cmd->writeDatagram(ParamCmd.toUtf8(), QHostAddress(ip), sendPort);

    QString str = "d";
    if (ui->checkBox_DSPOnoff->checkState() == Qt::Checked)
    {
        str += '1';
    }
    else
    {
        str += '0';
    }
    m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);

    ParamCmd = "f" + QString::number(ui->comboBox_upperCutoff->currentIndex()) + " " + QString::number(ui->comboBox_lowerCutoff->currentIndex());
}

void MainWindow::on_checkBox_DSPOnoff_stateChanged(int arg1)
{
    if (ui->checkBox_DSPOnoff->isChecked())
    {
        QString str = "d1";
        m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
    }
    else
    {
        QString str = "d0";
        m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
    }
}

void MainWindow::on_spinBox_samplingRate_valueChanged(int sampleRate)
{
    for (int i = 1; i < 16; i++)
    {
        ui->comboBox_DSP->setItemText(i, QString::number(sampleRate * kFreq[i - 1], 'g', 10) + " Hz");
    }
}

void MainWindow::on_pushButton_calibrate_clicked()
{
    QString str = "a";
    m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

void MainWindow::on_pushButton_clearCal_clicked()
{
    QString str = "c";
    m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

// /********** Register setting **********/

// void MainWindow::on_pushButton_read_clicked()
// {
//     recvType = UdpRead;
// //    QString str = "r" + QString::number(ui->spinBox_ReadAddr->value(), 10);
// //    m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
// }

// void MainWindow::on_pushButton_write_clicked()
// {
//     recvType = UdpRead;
// //    QString str = "w" + QString::number(ui->spinBox_writeAddr->value(), 10) + QString::number(ui->spinBox_writeData->value(), 10);
// //    m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
// }

/********** Device & Battery **********/
/**
 * @brief handle press run/stop
 *
 */
void MainWindow::on_pushButton_run_clicked()
{
    if (ui->pushButton_run->text() == "开始采集")
    {
        // send "s" to start accquisition process
        QString str = "s";
        m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);

        // switch button state(text)
        ui->pushButton_run->setText("停止采集");
    }
    else
    {
        // send "s" to start accquisition process
        QString str = "p";
        m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);

        // switch button state(text)
        ui->pushButton_run->setText("开始采集");
    }
}

void MainWindow::on_pushButton_savePath_clicked()
{
//    ShitFileName = QFileDialog::getOpenFileName(this, tr("open image file"), "./", tr("Image files(*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm);;All files (*.*)"));
    ShitFileName = QFileDialog::getSaveFileName(this, tr("选择保存文件目录..."), "", tr("(*.csv)"));
    if (!ShitFileName.isEmpty())
    {
        QFileInfo fileInfo(ShitFileName);

        if (fileInfo.isFile())
        {
            fileInfo.dir().remove(fileInfo.fileName());
        }
    }

}

void MainWindow::on_pushButton_getBatt_clicked()
{
    recvType = UdpBatt;
    QString str = "b";
    m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

void MainWindow::on_pushButton_shutdown_clicked()
{
    QString str = "o";
    m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
    ui->label_status->setText("设备状态：未连接");
}

void MainWindow::on_checkBox_plot_stateChanged(int arg1)
{
    if (ui->checkBox_plot->isChecked())
    {
        elecplot->setPlot_status(true);
    }
    else
    {
        elecplot->setPlot_status(false);
    }
}

/********** BLE & Impedance **********/

void MainWindow::on_pushButton_searchBLE_clicked()
{
    if (ui->pushButton_searchBLE->text() == QString("搜索蓝牙设备"))
    {
        ble_finder->show();
        ble_finder->scanDevice();
    }
    else
    {
        ble_finder->disconnectDevice();
        ui->label_bleState->setText(QString("状态：未连接"));
        ui->pushButton_searchBLE->setText(QString("搜索蓝牙设备"));
    }
}

void MainWindow::on_pushButton_startIMP_clicked()
{
    if (IMP_Status)     // Impedance measurement mode
    {
        ble_finder->setFreqCount(ui->spinBox_sweepPts->value());
        ble_finder->sendCommand(ConvertToByteArray("BF", (float)ui->doubleSpinBox_startFreq->value()));
        ble_finder->sendCommand(ConvertToByteArray("EF", (float)ui->doubleSpinBox_stopFreq->value()));
        ble_finder->sendCommand(ConvertToByteArray("EA", (float)ui->doubleSpinBox_amplitude->value()));
        ble_finder->sendCommand(ConvertToByteArray("PT", (int)ui->spinBox_sweepPts->value()));
        ble_finder->sendCommand(ConvertToByteArray("DR", (int)ui->spinBox_dataRate->value()));
    }
    else                // Waveform generation mode
    {
        ble_finder->sendCommand(ConvertToByteArray("BF", (float)ui->doubleSpinBox_startFreq->value()));
        ble_finder->sendCommand(ConvertToByteArray("EA", (float)ui->doubleSpinBox_amplitude->value()));
    }

    ble_finder->sendCommand(ConvertToByteArray("ST"));  // start process
}

void MainWindow::on_pushButton_channelSet_clicked()
{

}

void MainWindow::on_pushButton_switchMode_clicked()
{
    if (!IMP_Status)
    {
        IMP_Status = true;

        ble_finder->sendCommand(ConvertToByteArray("SM1"));

        ui->pushButton_switchMode->setText(QString("阻抗测量模式"));
        ui->label_startFreq->setText("开始频率");
        ui->doubleSpinBox_stopFreq->setEnabled(true);
        ui->spinBox_dataRate->setEnabled(true);
        ui->spinBox_sweepPts->setEnabled(true);
    }
    else
    {
        IMP_Status = false;

        ble_finder->sendCommand(ConvertToByteArray("SM2"));

        ui->pushButton_switchMode->setText(QString("波形发生模式"));
        ui->label_startFreq->setText("波形频率");
        ui->doubleSpinBox_stopFreq->setEnabled(false);
        ui->spinBox_dataRate->setEnabled(false);
        ui->spinBox_sweepPts->setEnabled(false);
    }
}

/********** Others **********/

void MainWindow::ActionDebug_triggered() {

    if (debugWindow == NULL)
    {
        debugWindow = new DebugWindow;
        debugWindow->setWindowTitle(QString("Debug"));
    }

    //    connect(debugWindow->, SIGNAL(triggered()), this, SLOT(on_ActionDebug_triggered()));
    debugWindow->show();
}

void MainWindow::BLE_DeviceConnected()
{
    ui->label_bleState->setText(QString("状态：已连接"));
    ui->pushButton_searchBLE->setText(QString("断开蓝牙连接"));
}

void MainWindow::on_pushButton_setPlotCh_clicked()
{
    elecplot->setPlotChannel(ui->comboBox_plotChannel->currentIndex());
}

uint8_t* MainWindow::ConvertToByteArray(QString string)
{
    QByteArray byteArray = string.toUtf8();
    uint8_t* data = (uint8_t*)malloc(byteArray.size());
    memcpy(data, byteArray.data(), byteArray.size());
    return data;
}

uint8_t* MainWindow::ConvertToByteArray(QString string, int value)
{
    QByteArray byteArray = string.toUtf8();
    uint8_t* Data = (uint8_t*)malloc(byteArray.size() + sizeof(value));

    memcpy(Data, byteArray.data(), byteArray.size());
    memcpy(Data + byteArray.size(), &value, sizeof(value));

    return Data;
}

uint8_t* MainWindow::ConvertToByteArray(QString string, float value)
{
    QByteArray byteArray = string.toUtf8();
    uint8_t* Data = (uint8_t*)malloc(byteArray.size() + sizeof(value));

    memcpy(Data, byteArray.data(), byteArray.size());
    memcpy(Data + byteArray.size(), &value, sizeof(value));

    return Data;
}


