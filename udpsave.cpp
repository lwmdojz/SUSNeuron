#include "udpsave.h"
#include "plot.h"

#include <QDebug>
#include <QSettings>
#include <QFileDialog>

udpSave::udpSave(QObject *parent)
    : QThread{parent}
{

}

void udpSave::run()
{
    QUdpSocket *mrecv = new QUdpSocket;
    QByteArray datagram;
    plotSize = 0;

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
                    if (plotSize != 0)
                    {
                        int j=0;
                        // channelPointer is odd, even byte to be plot => skip first byte
                        if ((channelPointer % 2) && !(plotSize % 2)) { j=1; }

                        for ( int i=j; i<datagram.size(); i++ )
                        {
                            plotData[channelPointer+i] = datagram[i];
                            plotSize--;

                            if (plotSize == 0)
                            {
                                qDebug() << "thread data send\n";
                                emit toPlot(plotData);
                                break;
                            }
                        }
                    }
                    // end of plot data processing

                    // channel pointer update
                    channelPointer = (channelPointer+datagram.size())%64;
                }
            }
        }
    }
    mrecv->close();
    mrecv->deleteLater();
}

void udpSave::getPlotData()
{
    qDebug() << "thread signal received\n";
    plotSize = 64;
}


void udpSave::stop()
{
    term = true;
}

void udpSave::udpInit(quint16 port, QString FileName)
{
    sendPort = port;
    fileName = FileName;
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













