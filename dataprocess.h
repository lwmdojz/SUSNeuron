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

    void processInit(quint16 newSampleRate, quint16 newNotchFreq);
    void setMaxPts(quint16 newSampleRate);
    void setNotchFreq(quint16 newNotchFreq);

signals:
    void toPlot(QList<QPointF> VoltagePts[32]);

public slots:
    void getRawData(QByteArray datagram);

protected:
    void run();
    
private:
    QList<QPointF> VoltagePts[32];

    quint16 sampleRate;
    quint16 notchFreq;

    quint16 MaxPts;
    quint64 timeStamp;

    QElapsedTimer ETimer;


};

#endif // DATAPROCESS_H
