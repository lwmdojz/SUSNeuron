#include "ble_finder.h"
#include "ui_ble_finder.h"

QLibrary* libblex = new QLibrary("libblex.dll");
QList<BlexPeripheral> peripherals;

BLE_Finder::BLE_Finder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BLE_Finder)
{
    ui->setupUi(this);

    if (libblex->load())
        qDebug() << "libblex.dll load success";
    else
        qDebug() << "libblex.dll load failed";

    // get Ble Adapter Handle
    typedef BlexAdapter (*FunDef)(size_t);
    FunDef blexAdapterGetHandle = (FunDef) libblex->resolve("blexAdapterGetHandle");
    adapter = blexAdapterGetHandle(0);

    // Register Scan Start/Stop
    typedef void (*callback)(BlexAdapter adapter, void* userdata);
    typedef BlexErrorCode (*SetCbFunc)(BlexAdapter, callback, void*);
    SetCbFunc SetCallbackOnScanStart = (SetCbFunc) libblex->resolve("blexAdapterSetCallbackOnScanStart");
    SetCbFunc blexAdapterSetCallbackOnScanStop = (SetCbFunc) libblex->resolve("blexAdapterSetCallbackOnScanStop");
    SetCallbackOnScanStart(adapter, callbackOnScanStart, nullptr);
    blexAdapterSetCallbackOnScanStop(adapter, callbackOnScanStart, nullptr);

    // Register Scan Found Callback
    typedef void (*callback2)(BlexAdapter adapter, BlexPeripheral peripheral, void* userdata);
    typedef BlexErrorCode (*SetCbFunc2)(BlexAdapter, callback2, void*);
    SetCbFunc2 SetCallbackOnScanFound = (SetCbFunc2) libblex->resolve("blexAdapterSetCallbackOnScanFound");
    SetCallbackOnScanFound(adapter, callbackOnScanFound, nullptr);

    scanStart();
}

BLE_Finder::~BLE_Finder()
{
    delete ui;
}

void BLE_Finder::addDevicesToList(QString identifier, QString address)
{
    ui->listWidget->addItem(identifier + " " + address);
}

void BLE_Finder::clearDevicesList(void)
{
    ui->listWidget->clear();
}

bool BLE_Finder::isBluetoothEnabled(void)
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

bool BLE_Finder::scanStart(void)
{
    typedef BlexErrorCode (*FunDef)(BlexAdapter);
    FunDef blexAdapterScanStart = (FunDef) libblex->resolve("blexAdapterScanStart");
    return blexAdapterScanStart(adapter);
}

bool BLE_Finder::scanStop(void)
{
    typedef BlexErrorCode (*FunDef)(BlexAdapter);
    FunDef blexAdapterScanStop = (FunDef) libblex->resolve("blexAdapterScanStop");
    return blexAdapterScanStop(adapter);
}

void BLE_Finder::on_pushButton_connect_clicked()
{

}

void BLE_Finder::on_pushButton_refresh_clicked()
{
    scanStart();
}

void callbackOnScanStart(BlexAdapter adapter, void* userdata) {}

void callbackOnScanFound(BlexAdapter adapter, BlexPeripheral peripheral, void* userdata)
{
    typedef char* (*FunDef)(BlexPeripheral handle);
    FunDef blexPeripheralIdentifier = (FunDef) libblex->resolve("blexPeripheralIdentifier");

    if (blexPeripheralIdentifier(peripheral)[0] != 0)
    {
        FunDef blexPeripheralAddress = (FunDef) libblex->resolve("blexPeripheralAddress");


        peripherals.append(peripheral);

        qDebug() << "Identifier: " << blexPeripheralIdentifier(peripheral);
        qDebug() << "Address: " << blexPeripheralAddress(peripheral);
    }
}

