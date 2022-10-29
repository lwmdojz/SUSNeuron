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
    // UDP init
    QUdpSocket *mrecv = new QUdpSocket;
    QByteArray datagram;

    // bind port 2334
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
                    // resize data buffer
                    datagram.resize(mrecv->pendingDatagramSize());

                    // get data from prot
                    mrecv->readDatagram(datagram.data(), datagram.size());

                    // send data to dataprocess
                    emit toProcess(datagram);

                    // save into file
                    outAppHeadBin.writeRawData(datagram.data(), datagram.size());
                }
            }
        }
    }
    // clean
    mrecv->close();
    mrecv->deleteLater();
}

void udpSave::stop() { term = true; }

// set function

/**
 * @brief set port for voltage data receive
 * 
 * @param port udp port for voltage data, 2334 by default
 */
void udpSave::setPort(quint16 port)
{
    sendPort = port;
}

/**
 * @brief set filename for voltage data
 * 
 * @param port file name for voltage data, "Vdata" by default
 */
void udpSave::setFilename(QString FileName)
{
    fileName = FileName;
}

/**
 * @brief get current file name for voltage data
 * 
 * @return QString, current file name
 */
QString udpSave::getFilename()
{
    return fileName;
}








