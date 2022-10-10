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

                    if (plotSize != 0)
                    {
                        for ( int i=0; i<64; i++ )
                        {
                            plotData[i] = datagram[i];

                        }
                        emit toPlot(plotData);
                        plotSize = 0;
                        // end of plot data processing
                    }
                }
            }
        }
    }
    mrecv->close();
    mrecv->deleteLater();
}

void udpSave::getPlotData()
{
//    qDebug() << "thread signal received\n";
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













