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

    void setPort(quint16 port);
    void setFilename(QString FileName);

    QString getFilename();

    bool term = false; 

signals:
    void toProcess(QByteArray datagram);

public slots:
    void

protected:
    void run();

private:
    quint16 sendPort = 2334;
    QString fileName = "";
};

#endif // UDPSAVE_H
