#include "mainwindow.h"
#include "ui_mainwindow.h"
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
        configIniWrite->setValue("ip", "192.168.1.2");
        ip = "192.168.1.2";
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
        QHostAddress cli_addr;  //对方地址
        quint16 port;  //对方端口
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

            ui->statusbar->showMessage(tr("应答完成!"),2000);
        } 
    });

    connect(mrecv, &QUdpSocket::readyRead, this, [=]()
    {
        //读取对方发送的数据
        char buf[2048] = {0};
        QHostAddress cli_addr;  //对方地址
        quint16 port;  //对方端口
        qint64 len = mrecv->readDatagram(buf, sizeof(buf), &cli_addr, &port);
        qDebug()<<"阻抗测试"<<len;
        if (len >0) {
            if(fileName!=""){
                QFile file(fileName);
                if (!(file.open(QFile::WriteOnly | QIODevice::Append)))//QIODevice::Append
                {
                    file.close();
                } else{
                    QTextStream out(&file);
                    for(int i=0;i<len/2;i++){
                        out<<((quint16)((quint8)buf[2*i+1])<<8)+(quint8)buf[2*i]<<"\n"; // data I want
                        qDebug()<<(quint8)buf[2*i+1]<<(quint8)buf[2*i];
                    }
                    file.close();

                }
            }
        } 
    });
    
    udp = new udpSave();
    ui->comboBox->clear();
    ui->comboBox->addItem(QString("0.1pF"));
    ui->comboBox->addItem(QString("1pF"));
    ui->comboBox->addItem(QString("10pF"));
    ui->comboBox->setCurrentIndex(2);
}

MainWindow::~MainWindow()
{
    if (udp->isRunning())
        udp->stop();
    delete ui;
}

void MainWindow::on_pushButton_SamplingPeriod_clicked()
{
    QString MyData = "m" + QString::number(ui->spinBox_SamplingPeriod->value(), 10);
    msend->writeDatagram(MyData.toUtf8(), QHostAddress(ip), sendPort);
}

void MainWindow::on_pushButton_DSPCutoff_clicked()
{
    QString MyData = "u" + QString::number(ui->spinBox_DSPCutoff->value(), 10);
    msend->writeDatagram(MyData.toUtf8(), QHostAddress(ip), sendPort);
}

void MainWindow::on_pushButton_DSPOnoff_clicked()
{
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
    QString str = "f" + QString::number(ui->spinBox_UpperBandwidth->value(), 10) + " " + QString::number(ui->spinBox_LowerBandwidth->value(), 10);
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
    if (ui->pushButton_run->text() == "运行")
    {
        QString str = "s";
        msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
        if (!udp->isRunning())
        {
            udp->term = false;
            udp->start();
        }
        ui->pushButton_run->setText("暂停");
    }
    else
    {
        QString str = "p";
        msend->writeDatagram(str.toUtf8(), QHostAddress(ip), sendPort);
        ui->pushButton_run->setText("运行");
        if (udp->isRunning())
            udp->stop();
    }
}

void MainWindow::on_pushButton_savePath_clicked()
{
    udp->fileName = QFileDialog::getSaveFileName(this, tr("保存文件（如果存在删除重建）"), "", tr("(*.dat)"));
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
    fileName = QFileDialog::getSaveFileName(this, tr("保存文件（如果存在删除重建）"), "", tr("(*.csv)"));
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
