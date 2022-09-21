#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "plot.h"

#include <QtCharts/QChartView>
#include <QtWidgets/QApplication>
#include <QFileDialog>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    mrecv = new QUdpSocket;
    msend = new QUdpSocket;
    qDebug() << "readBufferSize:" << msend->readBufferSize();
    msend->bind(sendPort);
    mrecv->bind(sendPort + 2);
    recvTpye = UdpOther;

    connect(msend, &QUdpSocket::readyRead, this, [=]()
    {
        char buf[1024] = {0};
        QHostAddress cli_addr;      // client address
        quint16 port;               // client port
        qint64 len = msend->readDatagram(buf, sizeof(buf), &cli_addr, &port);
        if (len > 0) {
            QString str = QString("[%1:%2] %3").arg(cli_addr.toString()).arg(port).arg(buf);
            qDebug()<<str;
            if(recvTpye==UdpRead)
            {
                quint8 reg=(quint8)buf[0];
                qDebug()<<buf[0];
                ui->label_readValue->setText(QString::number(reg,16));
            }
            if(recvTpye==UdpBatt)
            {
                quint16 reg=(quint8)buf[0]+(((quint16)buf[1])<<8);
                qDebug()<<reg;
                ui->label_batt->setText(QString::number(reg*2)+"mV");
            }
            recvTpye=UdpOther;

            ui->statusbar->showMessage(tr("Responsed!"),2000);
        } 
    });

    connect(mrecv, &QUdpSocket::readyRead, this, [=]()
    {
        // Keep receiving data
        char buf[2048] = {0};
        QHostAddress cli_addr;      // client address
        quint16 port;               // client port
        qint64 len = mrecv->readDatagram(buf, sizeof(buf), &cli_addr, &port);

        qDebug()<<"Impedance test"<<len;
        if (len >0) {
            if(fileName!=""){

                QFile file(fileName+".csv");

                if (!(file.open(QFile::WriteOnly | QIODevice::Text)))//QIODevice::Append
                {
                    file.close();
                } else{
                    QTextStream out(&file);
                    out.setEncoding(QStringConverter::Utf8);
                    out.generateByteOrderMark();

                    for(int i=0;i<32;i++)
                    {
                        out<<tr("ch")<<i<<",";
                    }

                    out << ((((quint16)buf[1])<<8)+(quint16)buf[0]) << ",";
                    plot->loadData(0, (((quint16)buf[1])<<8)+(quint16)buf[0]);

                    for(int i=1;i<len/2;i++){
                        out << ((((quint16)buf[1])<<8)+(quint16)buf[0]) << ",";
                        if(i%32 == 0) {
                            out << "\n";
                        }
                        plot->loadData(i%32, (((quint16)buf[1])<<8)+(quint16)buf[0]);
                    }
                    file.close();
                    plot->nextPt();
                }
            }
        } 
    });
    
    udp = new udpSave();

    ui->comboBox->setCurrentIndex(2);
    ui->comboBox_UpperCutoff->setCurrentIndex(3);
    ui->comboBox_LowerCutoff->setCurrentIndex(24);

    plot = new Plot;
    plot->setTitle("Dynamic Volt display");
    plot->legend()->hide();
    plot->setAnimationOptions(QChart::NoAnimation);
//    plot->setBackgroundVisible(false);
    plot->setContentsMargins(0, 0, 0, 0);
    plot->setMargins(QMargins(0, 0, 0, 0));
    plot->setBackgroundRoundness(0);

    ui->graphicsView->setChart(plot);
}

MainWindow::~MainWindow()
{
    if (udp->isRunning())
        udp->stop();
    delete ui;
}



void MainWindow::on_pushButton_DSPOnoff_clicked()
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

void MainWindow::on_pushButton_read_clicked()
{
    recvTpye = UdpRead;
    QString str = "r" + QString::number(ui->spinBox_Read->value(), 10);
    msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
}

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
        }
        ui->pushButton_run->setText("Running, press to stop");
        
        plot->startPlot();
    }
    else
    {
        QString str = "p";
        msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
        ui->pushButton_run->setText("Press to run");
        if (udp->isRunning())
            udp->stop();

        plot->pausePlot();
    }
}

void MainWindow::on_pushButton_savePath_clicked()
{
    udp->fileName = QFileDialog::getSaveFileName(this, tr("保存文件（如果存在删除重建）"), "", tr("(*.csv)"));
    QFileInfo fileInfo(udp->fileName);
    if (fileInfo.isFile())
    {
        fileInfo.dir().remove(fileInfo.fileName());
    }
    qDebug() << udp->fileName;
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
