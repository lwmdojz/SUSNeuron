#ifndef DATAPROCESS_H
#define DATAPROCESS_H

#include <QObject>
#include <QThread>

class dataprocess : public QThread
{
    Q_OBJECT
public:
    explicit dataprocess(QObject *parent = nullptr);

    void stop();
    void processInit(quint16 sampleRate, quint16 cutoffFreq);

signals:
    void PlotData();

public slots:
    void getRawData(QByteArray datagram);

protected:
    void run();
    
private:
    QList<quint16> VoltageChain[32];

    quint16 sampleRate;
    quint16 cutoffFreq;


};

#endif // DATAPROCESS_H
