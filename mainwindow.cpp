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

    // IP config
    QFileInfo iniFileInfo("config.ini"); // init, read .Ini file
    if (iniFileInfo.isFile())
    {
        QSettings *configIniRead = new QSettings("config.ini", QSettings::IniFormat); //初始化读取Ini文件对象
        ip = configIniRead->value("ip").toString();
        qDebug() << configIniRead->value("ip").toString();
    }
    else
    {
        QSettings *configIniWrite = new QSettings("config.ini", QSettings::IniFormat);
        configIniWrite->setValue("ip", "192.168.137.24");
        ip = "192.168.137.24";
    }

    // UDP init
    mrecv = new QUdpSocket;
    msend = new QUdpSocket;
    msend->bind(sendPort);     // for normal commend
    mrecv->bind(sendPort + 2); // for impedance test
    recvTpye = UdpOther;

    connect(msend, &QUdpSocket::readyRead, this, &MainWindow::handleSend);

    connect(mrecv, &QUdpSocket::readyRead, this, &MainWindow::handleReceive);

    ui->comboBox->setCurrentIndex(2);
    ui->comboBox_UpperCutoff->setCurrentIndex(3);
    ui->comboBox_LowerCutoff->setCurrentIndex(24);

    plot = new Plot;
    plot->setTitle("Dynamic Volt display");
    plot->legend()->hide();
    plot->setAnimationOptions(QChart::NoAnimation);
    plot->setContentsMargins(0, 0, 0, 0);
    plot->setMargins(QMargins(0, 0, 0, 0));
    plot->setBackgroundRoundness(0);

    udp = new udpSave();

    ui->graphicsView->setChart(plot);
//    ui->graphicsView->setRubberBand(QChartView::RectangleRubberBand);

//    connect(plot, &Plot::getPlotData, udp, &udpSave::getPlotData);
    connect(udp, &udpSave::toPlot, plot, &Plot::plotData);
}

MainWindow::~MainWindow()
{
    if (udp->isRunning())
        udp->stop();
    delete ui;
}

// Udp handle function

void MainWindow::handleSend()
{
    char buf[1024] = {0};
    QHostAddress cli_addr; // client address
    quint16 port;          // client port
    qint64 len = msend->readDatagram(buf, sizeof(buf), &cli_addr, &port);

    if (len > 0)
    {
        QString str = QString("[%1:%2] %3").arg(cli_addr.toString()).arg(port).arg(buf);
        qDebug() << str;
        if (recvTpye == UdpRead)
        {
            QString reg = "0x" + QString::number((quint8)buf[0], 16);
            qDebug() << reg;
            ui->label_readValue->setText(reg);
        }
        if (recvTpye == UdpBatt)
        {
            QString bat = QString::number(((quint8)buf[0]+(((quint16)buf[1])<<8)) * 2) + "mV";
            qDebug() << bat;
            ui->label_batt->setText(bat);
        }
        recvTpye = UdpOther;

        ui->statusbar->showMessage(tr("Responsed!"), 1000);
    }
}

// Read data of Impedance test
void MainWindow::handleReceive()
{
    char buf[2048] = {0};
    QHostAddress cli_addr; // Client address
    quint16 port;          // Client port
    qint64 len = mrecv->readDatagram(buf, sizeof(buf), &cli_addr, &port);
    qDebug() << "Impedance test" << len;

    if (len > 0)
    {
        if (fileName != "")
        {
            QFile file(fileName);
            if (!(file.open(QFile::WriteOnly | QIODevice::Append))) // QIODevice::Append
            {
                file.close();
            }
            else
            {
                QTextStream out(&file);
                for (int i = 0; i < len / 2; i++)
                {
                    out << ((quint16)((quint8)buf[2 * i + 1]) << 8) + (quint8)buf[2 * i] << "\n"; // data I want
                    qDebug() << (quint8)buf[2 * i + 1] << (quint8)buf[2 * i];
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

// Acquisition action setting

void MainWindow::on_pushButton_run_clicked()
{
    if (ui->pushButton_run->text() == "Press to run")
    {
        QString str = "s";
        msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
        if (!udp->isRunning())
        {
            udp->term = false;
            udp->start();
//            plot->plotTimer.start();
        }
        ui->pushButton_run->setText("Running, press to stop");
    }
    else
    {
        QString str = "p";
        msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
        ui->pushButton_run->setText("Press to run");
        if (udp->isRunning())
        {
            udp->stop();
//            plot->plotTimer.stop();
        }
    }
}

void MainWindow::on_pushButton_savePath_clicked()
{
    udp->setFilename(QFileDialog::getSaveFileName(this, tr("Save as...(if file existed, program will delete and create a new one)"), "", tr("(*.dat)")));
    QFileInfo fileInfo(udp->getFilename());

    if (fileInfo.isFile())
    {
        fileInfo.dir().remove(fileInfo.fileName());
    }
    qDebug() << udp->getFilename();
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
    fileName = QFileDialog::getSaveFileName(this, tr("Save file (if file existed, delete and rebuild it"), "", tr("(*.csv)"));
    QFileInfo fileInfo(fileName);
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




