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
    m_cmd->bind(sendPort);      // for normal commend, 2333

    m_sig = new QUdpSocket;
    m_sig->bind(shitPort);      // for shit data, 2334

    m_imp = new QUdpSocket;
    m_imp->bind(recvPort);      // for impedance test, 2335

    recvType = UdpOther;

    // signal-slot, read data - handle
    connect(m_cmd, &QUdpSocket::readyRead, this, &MainWindow::handleCommand);
    connect(m_sig, &QUdpSocket::readyRead, this, &MainWindow::handleElecSig);
    connect(m_imp, &QUdpSocket::readyRead, this, &MainWindow::handleImpedance);

    // set default choice of comvo boxes
    ui->comboBox_UpperCutoff->setCurrentIndex(3);
    ui->comboBox_LowerCutoff->setCurrentIndex(24);

    // DSP cutoff frequency calculate
    for (int i = 1; i < 16; i++)
    {
        ui->comboBox_DSP->setItemText(i,
            QString::number(ui->spinBox_SamplingRate->value() * kFreq[i-1], 'g', 10) + " Hz");
    }
    ui->comboBox_DSP->setCurrentIndex(6);

    // signal-slot, Debug window
    connect(ui->actionDebug, SIGNAL(triggered()), this, SLOT(ActionDebug_triggered()));

    // init Plot object
    plot = new Plot;

    ble_imp = new BLE_IMP;
    ble_imp->isBluetoothEnabled();

    // set plot to graphicsView on panel
    ui->graphicsView->setChart(plot);
    ui->graphicsView->setRubberBand(QChartView::RectangleRubberBand);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_searchIP_clicked()
{
    // To Do: IP Search
    QString ip_temp = "192.168.137.0";      // IP address in QString

    for (quint16 i = 0; i < 256; i++)
    {
        recvType = UdpSearch;
        ip_temp = "192.168.137." + QString::number(i);
        QString ParamCmd = "m" + QString::number(ui->spinBox_SamplingRate->value(), 10);
        m_cmd->writeDatagram(ParamCmd.toUtf8(), QHostAddress(ip_temp), sendPort);

        Delay_MSec(10);

        if (connect_status)
        {
            break;
        }
        
    }
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
        qDebug() <<  QString("[%1:%2] %3").arg(cli_addr.toString()).arg(port).arg(buf);

        // read register feedback
        if (recvType == UdpRead)
        {
            QString reg = "0x" + QString::number((quint8)buf[0], 16);
            qDebug() << reg;
        }
        // Battery voltage feedback
        else if (recvType == UdpBatt)
        {
            QString bat = QString::number(((quint8)buf[0]+(((quint16)buf[1])<<8)) * 2) + " mV";
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

        ui->statusbar->showMessage(tr("Responsed!"), 1500);
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

    if (len > 0)    // get feedback
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

    if (len > 0)            // get feedback
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

                    if (i%32==0 && i!=0)
                    {
                        out << "\n";
                    }
                }

                if (plot->getPlot_status() == true)
                {
                    plot->Plotting(temp, len/2);
                }
                file.close();
            }
        }
    }
}


//void MainWindow::BLE

/********** Acquisition parameter setting **********/

void MainWindow::on_pushButton_SetParam_clicked()
{
    QString ParamCmd = "m" + QString::number(ui->spinBox_SamplingRate->value(), 10);
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

    ParamCmd = "f" + QString::number(ui->comboBox_UpperCutoff->currentIndex()) + " " + QString::number(ui->comboBox_LowerCutoff->currentIndex());
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

void MainWindow::on_pushButton_calibrate_clicked()
{
    QString str = "a";
    m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

void MainWindow::on_pushButton_clear_clicked()
{
    QString str = "c";
    m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

/********** Register setting **********/

void MainWindow::on_pushButton_read_clicked()
{
    recvType = UdpRead;
//    QString str = "r" + QString::number(ui->spinBox_ReadAddr->value(), 10);
//    m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

void MainWindow::on_pushButton_write_clicked()
{
    recvType = UdpRead;
//    QString str = "w" + QString::number(ui->spinBox_writeAddr->value(), 10) + QString::number(ui->spinBox_writeData->value(), 10);
//    m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}


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
    ShitFileName = QFileDialog::getSaveFileName(this, tr("Save as...(if file existed, program will delete and create a new one)"), "", tr("(*.csv)"));
    QFileInfo fileInfo(ShitFileName);

    if (fileInfo.isFile())
    {
        fileInfo.dir().remove(fileInfo.fileName());
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

/********** Impedance measurement setting **********/

//void MainWindow::on_pushButton_channelset_clicked()
//{
////    QString str = "h" + QString::number(ui->spinBox_ch->value(), 10);-
//    m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
//}

//void MainWindow::on_pushButton_currentset_clicked()
//{
////    QString str = "i" + QString::number(ui->comboBox_->currentIndex(), 10);
////    m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
//}

void MainWindow::on_pushButton_impedancetest_clicked()
{
    ImpedanceFileName = QFileDialog::getSaveFileName(this, tr("Save file (if file existed, delete and rebuild it"), "", tr("(*.csv)"));
    QFileInfo fileInfo(ImpedanceFileName);
    if (fileInfo.isFile())
    {
        fileInfo.dir().remove(fileInfo.fileName());
    }
    QString str = "z";
    m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

//void MainWindow::on_pushButton_hz_clicked()
//{

////    double a = ui->doubleSpinBox_hz->value();
//    int b = a * 10;
//    QString str = "g" + QString::number(b, 10);
//    m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
//}

//void MainWindow::on_pushButton_zcount_clicked()
//{
////    QString str = "j" + QString::number(ui->spinBox_zcount->value(), 10);
//    m_cmd->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
//}

void MainWindow::on_pushButton_setPlotCh_clicked()
{
    plot->setPlotChannel(ui->comboBox_plotChannel->currentIndex());
}



void MainWindow::ActionDebug_triggered()
{
    debugWindow = new DebugWindow;
    debugWindow->setWindowTitle(QString("Debug"));

    //    connect(debugWindow->, SIGNAL(triggered()), this, SLOT(on_ActionDebug_triggered()));
    debugWindow->show();
}

void MainWindow::on_checkBox_plot_stateChanged(int arg1)
{
    if (ui->checkBox_plot->isChecked())
    {
        plot->setPlot_status(true);
    }
    else
    {
        plot->setPlot_status(false);
    }
}


void MainWindow::on_spinBox_SamplingRate_valueChanged(int sampleRate)
{
    for (int i = 1; i < 16; i++)
    {
        ui->comboBox_DSP->setItemText(i, QString::number(sampleRate*kFreq[i-1], 'g', 10) + " Hz");
    }
}

void MainWindow::Delay_MSec(int msec)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}
