#ifndef BLE_IMP_H
#define BLE_IMP_H

#include <QLibrary>
#include <QtCore>

class BLE_IMP
{
public:
    BLE_IMP();
    ~BLE_IMP();

    bool isBluetoothEnabled();
    
private:
    QLibrary* libblex;
};

#endif // BLE_IMP_H
