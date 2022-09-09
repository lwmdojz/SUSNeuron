#include "udpsave.h"
#include <QDebug>
#include <QSettings>
#include <QFileDialog>
udpSave::udpSave(QObject *parent)
    : QThread{parent}
{
//    QFileInfo iniFileInfo("config.ini");
//    if(iniFileInfo.isFile())
//    {
//        QSettings * configIniRead = new QSettings("config.ini",QSettings::IniFormat);//初始化读取Ini文件对象
//        ip=configIniRead->value("ip").toString();
//        qDebug()<<configIniRead->value("ip").toString();
//    }else{
//        ip="192.168.1.2";
//    }
//    qDebug()<<"udpSave"<<ip;

}

void udpSave::run()
{
    QUdpSocket  *mrecv = new QUdpSocket;
    QByteArray datagram;

    int receive = mrecv->bind(2334);
    qDebug() << "receive: " <<receive;

    if(fileName!=""){
        QFile mdata(fileName);
        if (!(mdata.open(QFile::WriteOnly | QIODevice::Append)))//QIODevice::Append
        {
            mdata.close();
        }
        else{
            while(!term){
                 QDataStream outAppHeadBin(&mdata);
                while(mrecv->hasPendingDatagrams()){
                    datagram.resize(mrecv->pendingDatagramSize());

                    mrecv->readDatagram(datagram.data(),datagram.size());
//                     qDebug()<<(uint8_t)datagram.data()[0]<<(uint8_t)datagram.data()[2]<<(uint8_t)datagram.data()[3];
//                    qDebug()<<datagram;
//                    QTextStream out(&mdata);
//                    out << datagram;
                    outAppHeadBin.writeRawData(datagram.data(),datagram.size());
                }
            }
        }
    }


    mrecv->close();
    mrecv->deleteLater();
}

void udpSave::stop()
{
    term=true;
}
