#ifndef UDPSAVE_H
#define UDPSAVE_H

#include <QFile>
#include <QThread>
#include <QUdpSocket>
#include <QtCore>

#include "plot.h"

extern quint8 plotData[65];

class udpSave : public QThread
{
    Q_OBJECT

public:
    explicit udpSave(QObject *parent = nullptr);

    void stop();

    void binarySave();
    void plotSave();

    void udpInit(quint16 port, QString FileName);
    void setPort(quint16 port);
    void setFilename(QString FileName);
    void setPlotAddr(Plot *plotAddr);

    QString getFilename();

    void handleTimeout();

    bool term = false;

    QTimer udpTimer;


protected:
    void run();

private:

    quint16 sendPort = 2334;
    QString fileName = "";

    quint16 channelPointer;

    quint16 plotSize;

};

#endif // UDPSAVE_H
