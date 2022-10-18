#include "dataprocess.h"

dataprocess::dataprocess(QObject *parent)
    : QThread{parent}
{

}


void dataprocess::run()
{
    if (VoltageChain[0].size() < 3000)
    {
        quit();
    }

    for (int i = 0; i < sampleRate; i++)
    {
        
    }
}

void dataprocess::getRawData(QByteArray datagram)
{
    for (quint16 i = 0; i < datagram.size(); i+=2)
    {
        VoltageChain[i%64/2].append(((quint16)(datagram[i+1])<<8)+(quint16)datagram[i]);
    }
}

void processInit(quint16 Sample_Rate, quint16 Cutoff_Frequency)
{
    sampleRate = Sample_Rate;
    cutoffFreq = Cutoff_Frequency;
}


