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
    

signals:

};

#endif // DATAPROCESS_H
