#ifndef UDPSAVE_H
#define UDPSAVE_H

#include <QFile>
#include <QThread>
#include <QUdpSocket>
#include <QtCore>


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

    QString getFilename();

    bool term = false;


protected:
    void run();

private:

    quint16 sendPort = 2334;
    QString fileName = "";

    quint16 channelPointer = 0;

};

#endif // UDPSAVE_H
