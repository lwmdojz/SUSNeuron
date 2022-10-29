#include "dataprocess.h"
#include "qpoint.h"

dataprocess::dataprocess(QObject *parent)
    : QThread{parent}
{

}

void dataprocess::run()
{
    // notch filter

    // trigger signal, transfer data to main plotting thread
    emit toPlot(VoltagePts);
}


void dataprocess::processInit(quint16 newSampleRate, quint16 newNotchFreq)
{
    // maximum # of points: 60s * sample rate 
    MaxPts = newSampleRate*60;

    // notch filter frequency
    notchFreq = newNotchFreq;
}

void dataprocess::setNotchFreq(quint16 newNotchFreq)
{
    notchFreq = newNotchFreq;
}

void dataprocess::setMaxPts(quint16 newSampleRate)
{
    MaxPts = newSampleRate*60;
}

void dataprocess::getRawData(QByteArray datagram)
{
    // estimate time
    ETimer.start();

    for (quint16 i = 0; i < datagram.size(); i+=2)
    {
        quint16 n = i%64/2;

        qreal Volt =((quint16)(datagram[i+1])<<8) + (quint16)datagram[i];
        Volt = Volt*10/65535 - 5;

        VoltagePts[n].append(QPointF(timeStamp, Volt));

        if (VoltagePts[n].size() > MaxPts)
        {
            VoltagePts[n].removeFirst();
        }
    }
    this->run();
}




