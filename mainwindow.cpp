#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "plot.h"

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
    else
    {
        QSettings *configIniWrite = new QSettings("config.ini", QSettings::IniFormat);
        configIniWrite->setValue("ip", "192.168.137.24");
        ip = "192.168.137.24";
    }

    // UDP init
    msend = new QUdpSocket;
    msend->bind(sendPort);      // for normal commend, 2333

    mshit = new QUdpSocket;
    mshit->bind(shitPort);      // for shit data, 2334

    mrecv = new QUdpSocket;
    mrecv->bind(recvPort);      // for impedance test, 2335

    recvTpye = UdpOther;

    // signal-slot, read data - handle
    connect(msend, &QUdpSocket::readyRead, this, &MainWindow::handleSend);
    connect(mshit, &QUdpSocket::readyRead, this, &MainWindow::handleShit);
    connect(mrecv, &QUdpSocket::readyRead, this, &MainWindow::handleReceive);

    // set default choice of comvo boxes
    ui->comboBox->setCurrentIndex(2);
    ui->comboBox_UpperCutoff->setCurrentIndex(3);
    ui->comboBox_LowerCutoff->setCurrentIndex(24);

    // init Plot object
    plot = new Plot;

    // set plot to graphicsView on panel
    ui->graphicsView->setChart(plot);
    ui->graphicsView->setRubberBand(QChartView::RectangleRubberBand);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Udp handle function

/**
 * @brief handle send command feedback
 */
void MainWindow::handleSend()
{
    QHostAddress cli_addr; // client address
    quint16 port;          // client port

    // init buffer
    char buf[1024] = {0};
    qint64 len = msend->readDatagram(buf, sizeof(buf), &cli_addr, &port);

    if (len > 0)
    {
        // print debug data
        qDebug() <<  QString("[%1:%2] %3").arg(cli_addr.toString()).arg(port).arg(buf);

        // normal feedback, should be "a"
        if (recvTpye == UdpRead)
        {
            QString reg = "0x" + QString::number((quint8)buf[0], 16);
            qDebug() << reg;
            ui->label_readValue->setText(reg);
        }
        // Battery voltage feedback
        else if (recvTpye == UdpBatt)
        {
            QString bat = QString::number(((quint8)buf[0]+(((quint16)buf[1])<<8)) * 2) + "mV";
            qDebug() << bat;
            ui->label_batt->setText(bat);
        }
        recvTpye = UdpOther;

        ui->statusbar->showMessage(tr("Responsed!"), 1000);
    }
}

/**
 * @brief Handle Impedance test data
 */
void MainWindow::handleReceive()
{
    QHostAddress cli_addr; // Client address
    quint16 port;          // Client port

    // buffer init
    char buf[2048] = {0};
    qint64 len = mrecv->readDatagram(buf, sizeof(buf), &cli_addr, &port);

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

void MainWindow::handleShit()
{
    QHostAddress cli_addr; // Client address
    quint16 port;          // Client port

    // buffer init
    char buf[2082] = {0};
    qint32 len = mshit->readDatagram(buf, sizeof(buf), &cli_addr, &port);

    if (len > 0)    // get feedback
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
                for (int i = 0; i < len / 2; i++)
                {
                    quint16 temp = ((quint16)((quint8)buf[2 * i + 1]) << 8) + (quint8)buf[2 * i];
                    out << temp << ",";
//                    qDebug() << temp;

                    if (i%32 == plot->getPlotChannel())
                    {
                        plot->Plotting(temp);
                    }

                    if (i%32==0 && i!=0)
                    {
                        out << "\n";
                    }
                }
                file.close();
            }
        }
    }
}


// Acquisition parameter setting

void MainWindow::on_pushButton_SampleDSP_clicked()
{
    QString MyData = "m" + QString::number(ui->spinBox_SamplingPeriod->value(), 10);
    msend->writeDatagram(MyData.toUtf8(), QHostAddress(ip), sendPort);

    MyData = "u" + QString::number(ui->spinBox_DSPCutoff->value(), 10);
    msend->writeDatagram(MyData.toUtf8(), QHostAddress(ip), sendPort);

    QString str = "d";
    if (ui->checkBox_DSPOnoff->checkState() == Qt::Checked)
    {
        str += '1';
    }
    else
    {
        str += '0';
    }
    msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

void MainWindow::on_pushButton_Bandwidth_clicked()
{
    QString str = "f" + QString::number(ui->comboBox_UpperCutoff->currentIndex()) + " " + QString::number(ui->comboBox_LowerCutoff->currentIndex());
    msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

void MainWindow::on_pushButton_calibrate_clicked()
{
    QString str = "a";
    msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

void MainWindow::on_pushButton_clear_clicked()
{
    QString str = "c";
    msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

// Register setting

void MainWindow::on_pushButton_read_clicked()
{
    recvTpye = UdpRead;
    QString str = "r" + QString::number(ui->spinBox_ReadAddr->value(), 10);
    msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

void MainWindow::on_pushButton_write_clicked()
{
    recvTpye = UdpRead;
    QString str = "w" + QString::number(ui->spinBox_writeAddr->value(), 10) + QString::number(ui->spinBox_writeData->value(), 10);
    msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

/**
 * @brief handle press run/stop
 * 
 */
void MainWindow::on_pushButton_run_clicked()
{
    if (ui->pushButton_run->text() == "Press to run")
    {
        // send "s" to start accquisition process
        QString str = "s";
        msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);

        // switch button state(text)
        ui->pushButton_run->setText("Running, press to stop");
    }
    else
    {
        // send "s" to start accquisition process
        QString str = "p";
        msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);

        // switch button state(text)
        ui->pushButton_run->setText("Press to run");
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
    recvTpye = UdpBatt;
    QString str = "b";
    msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

void MainWindow::on_pushButton_shutdown_clicked()
{
    QString str = "o";
    msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

// Impedance measurement setting

void MainWindow::on_pushButton_channelset_clicked()
{
    QString str = "h" + QString::number(ui->spinBox_ch->value(), 10);
    msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

void MainWindow::on_pushButton_currentset_clicked()
{
    QString str = "i" + QString::number(ui->comboBox->currentIndex(), 10);
    msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

void MainWindow::on_pushButton_impedancetest_clicked()
{
    ImpedanceFileName = QFileDialog::getSaveFileName(this, tr("Save file (if file existed, delete and rebuild it"), "", tr("(*.csv)"));
    QFileInfo fileInfo(ImpedanceFileName);
    if (fileInfo.isFile())
    {
        fileInfo.dir().remove(fileInfo.fileName());
    }
    QString str = "z";
    msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

void MainWindow::on_pushButton_hz_clicked()
{

    double a = ui->doubleSpinBox_hz->value();
    int b = a * 10;
    QString str = "g" + QString::number(b, 10);
    msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

void MainWindow::on_pushButton_zcount_clicked()
{
    QString str = "j" + QString::number(ui->spinBox_zcount->value(), 10);
    msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}


void MainWindow::on_pushButton_setPlotCh_clicked()
{
    plot->setPlotChannel(ui->comboBox_plotCh->currentIndex());
}

