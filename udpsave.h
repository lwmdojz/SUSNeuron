#ifndef UDPSAVE_H
#define UDPSAVE_H
#include <QFile>
#include <QThread>
#include <QUdpSocket>
class udpSave : public QThread
{
    Q_OBJECT
public:
    explicit udpSave(QObject *parent = nullptr);
    void stop();
    QString fileName="";
    bool term = false;
protected:
    void run();

private:

//    QString ip="192.168.137.102";
    quint16 sendPort=2334;
};

#endif // UDPSAVE_H
