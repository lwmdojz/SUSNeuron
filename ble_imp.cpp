#include "ble_imp.h"

BLE_IMP::BLE_IMP()
{
    libblex = new QLibrary("libblex.dll");
    if (libblex->load())
        qDebug() << "libblex.dll load success";
    else
        qDebug() << "libblex.dll load failed";

    // typedef int (*FunDef)(void);
    // FunDef blexAdapterGetHandle = (FunDef) libblex->resolve("blexAdapterGetHandle");

    // typedef int (*FuncDef)(int);
    // FuncDef blexAdapterScanStart = (FuncDef) libblex->resolve("blexAdapterScanStart");

    // qDebug() << "blexAdapterStartStatus: " << blexAdapterScanStart(blexAdapterGetHandle());
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


