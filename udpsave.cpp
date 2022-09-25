#include "udpsave.h"
#include <QDebug>
#include <QSettings>
#include <QFileDialog>

udpSave::udpSave(QObject *parent)
    : QThread{parent}
{
    QObject::connect(&udpTimer, &QTimer::timeout, this, &udpSave::handleTimeout);
    udpTimer.setInterval(100); // 50 frame per second
}

void udpSave::run()
{
    QUdpSocket *mrecv = new QUdpSocket;
    QByteArray datagram;

    mrecv->bind(2334);

    if (fileName != "")
    {
        QFile mdata(fileName);
        if (!(mdata.open(QFile::WriteOnly | QIODevice::Append)))
        {
            mdata.close();
        }
        else
        {
            while ( !term )
            {
                QDataStream outAppHeadBin(&mdata);

                while ( mrecv->hasPendingDatagrams() )
                {
                    datagram.resize(mrecv->pendingDatagramSize());

                    mrecv->readDatagram(datagram.data(), datagram.size());
                    outAppHeadBin.writeRawData(datagram.data(), datagram.size());

                    // if data to be plotted > 0
                    if (plotSize)
                    {
                        int j=0;
                        // channelPointer is odd => skip first byte
                        if ( channelPointer % 2 )
                        {
                            j=1;
                            // odd byte to be plot => store first byte as high
                            if ( plotSize % 2 )
                            {
                                plotData[channelPointer] = datagram[channelPointer];
                                channelPointer = (channelPointer+1) % 64;
                            }
                        }

                        for ( int i=0; i<(datagram.size()-j)/2; i++ )
                        {
                            plotData[channelPointer] = datagram[channelPointer];
                            channelPointer = (channelPointer+1) % 64;

                            if (!plotSize)
                            {
                                plotData[64] = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    mrecv->close();
    mrecv->deleteLater();
}

void udpSave::stop()
{
    term = true;
}

void udpSave::udpInit(quint16 port, QString FileName)
{
    sendPort = port;
    fileName = FileName;
//    plot = plotAddr;
}

void udpSave::setPort(quint16 port)
{
    sendPort = port;
}

void udpSave::setFilename(QString FileName)
{
    fileName = FileName;
}

QString udpSave::getFilename()
{
    return fileName;
}

void udpSave::handleTimeout()
{
    plotSize += 64;
}













