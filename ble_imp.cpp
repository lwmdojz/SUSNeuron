#include "ble_imp.h"

BLE_IMP::BLE_IMP()
{
    libblex = new QLibrary("libblex.dll");
    if (libblex->load())
        qDebug() << "libblex.dll load success";
    else
        qDebug() << "libblex.dll load failed";

    size_t size = 1;

    typedef BlexAdapter (*FunDef)(size_t);
    FunDef blexAdapterGetHandle = (FunDef) libblex->resolve("blexAdapterGetHandle");

    typedef void (*callback)(void);

    typedef BlexErrorCode (*FuncDef)(BlexAdapter, callback, int*);
    FuncDef blexAdapterSetCallbackOnScanStart = (FuncDef) libblex->resolve("blexAdapterSetCallbackOnScanStart");

    if (SIMPLEBLE_SUCCESS == blexAdapterSetCallbackOnScanStart(blexAdapterGetHandle(size), callbackOnScanStart, NULL) )
    {
        qDebug() << "blexAdapterSetCallbackOnScanStart success";
    }
    
    
}

void BLE_IMP::callbackOnScanStart(void)
{
    // do what?
}

BLE_IMP::~BLE_IMP() {}

bool BLE_IMP::isBluetoothEnabled(void)
{
    typedef bool (*FunDef)(void);
    FunDef blexAdapterIsBluetoothEnabled = (FunDef) libblex->resolve("blexAdapterIsBluetoothEnabled");

    if (blexAdapterIsBluetoothEnabled())
    {
        qDebug() << "Bluetooth is enabled";
        return true;
    }
    else
    {
        qDebug() << "Bluetooth is disabled";
        return false;
    }
}


