#include "ble_finder.h"
#include "ui_ble_finder.h"

static QLibrary* libblex = new QLibrary("libblex.dll");
static QList<BlexPeripheral> searchedPeripherals;

BLE_Finder::BLE_Finder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BLE_Finder)
{
    ui->setupUi(this);

    if (libblex->load())
        qDebug() << "libblex.dll load success";
    else
        qDebug() << "dll load failed";

    /* get Ble Adapter Handle */    {
        typedef BlexAdapter (*FunDef)(size_t);
        FunDef blexAdapterGetHandle = (FunDef)libblex->resolve("blexAdapterGetHandle");
        adapter = blexAdapterGetHandle(0);
    }

    service = new BlexService;

    /* Register Scan Functions */   {
        typedef void (*callback)(BlexAdapter adapter, void* userdata);
        typedef BlexErrorCode (*SetCbFunc)(BlexAdapter, callback, void*);
        SetCbFunc SetCallbackOnScanStart = (SetCbFunc)libblex->resolve("blexAdapterSetCallbackOnScanStart");
        SetCbFunc blexAdapterSetCallbackOnScanStop = (SetCbFunc)libblex->resolve("blexAdapterSetCallbackOnScanStop");
        SetCallbackOnScanStart(adapter, callbackOnScanStart, nullptr);
        blexAdapterSetCallbackOnScanStop(adapter, callbackOnScanStart, nullptr);

        // Register Scan Found Callback
        typedef void (*callback2)(BlexAdapter adapter, BlexPeripheral peripheral, void* userdata);
        typedef BlexErrorCode (*SetCbFunc2)(BlexAdapter, callback2, void*);
        SetCbFunc2 SetCallbackOnScanFound = (SetCbFunc2)libblex->resolve("blexAdapterSetCallbackOnScanFound");
        SetCallbackOnScanFound(adapter, callbackOnScanFound, nullptr);
    }

    // deviceScan();
}

BLE_Finder::~BLE_Finder()
{
    delete ui;
    close();
}

/* Self defined functions */

bool BLE_Finder::isBluetoothEnabled(void)
{
    typedef bool (*FunDef)(void);
    FunDef blexAdapterIsBluetoothEnabled = (FunDef)libblex->resolve("blexAdapterIsBluetoothEnabled");

    return (bool)blexAdapterIsBluetoothEnabled();
}

bool BLE_Finder::isBluetoothConnected()
{
    if (peripheral == nullptr)
    {
        return false;
    }
    return true;
}

void BLE_Finder::scanDevice(void)
{
    typedef BlexErrorCode (*FunDef)(BlexAdapter);
    FunDef blexAdapterScanStart = (FunDef)libblex->resolve("blexAdapterScanStart");
    FunDef blexAdapterScanStop = (FunDef)libblex->resolve("blexAdapterScanStop");

    blexAdapterScanStart(adapter);
    
    int count_t = 0;
    for (int i = 0; i < 100; i++)       // Scan for 10s
    {
        Delay_MSec(100);
        if (searchedPeripherals.size() > count_t)
        {
            typedef char* (*FunDef)(BlexPeripheral);
            FunDef blexPeripheralIdentifier = (FunDef)libblex->resolve("blexPeripheralIdentifier");
            FunDef blexPeripheralAddress = (FunDef)libblex->resolve("blexPeripheralAddress");

            while (count_t < searchedPeripherals.size())
            {
                ui->listWidget->addItem(QString(blexPeripheralIdentifier(searchedPeripherals[count_t])) + "\n                          " +
                                        QString(blexPeripheralAddress(searchedPeripherals[count_t])));
                count_t++;
            }
        }
    }
    
    blexAdapterScanStop(adapter);
}

void BLE_Finder::getService(BlexPeripheral blexPeripheral, QString uuid)
{
    typedef BlexErrorCode (*FunDef)(BlexPeripheral);
    FunDef blexPeripheralGetServicesCount = (FunDef)libblex->resolve("blexPeripheralGetServicesCount");

    typedef BlexErrorCode (*FuncDef)(BlexPeripheral, size_t, BlexService*);
    FuncDef blexPeripheralGetServices = (FuncDef)libblex->resolve("blexPeripheralGetServices");

    BlexService* blexService = new BlexService;

    qDebug() << blexPeripheralGetServicesCount(blexPeripheral);

    for (int i = 0; i < blexPeripheralGetServicesCount(blexPeripheral); i++) {
        blexPeripheralGetServices(blexPeripheral, i, blexService);
        if (QString(blexService->uuid.value) == uuid)
        {
            qDebug() << "Service uuid: " << blexService->uuid.value;
            service = blexService;

            return;
        }
    }
    qDebug() << "no such service";
}

void BLE_Finder::connectDevice(void)
{
    typedef BlexErrorCode (*FunDef)(BlexPeripheral);
    FunDef blexPeripheralConnect = (FunDef)libblex->resolve("blexPeripheralConnect");

    if (!blexPeripheralConnect(searchedPeripherals[ui->listWidget->currentRow()]))
    {
        peripheral = searchedPeripherals[ui->listWidget->currentRow()];
        qDebug() << "BLE Connected";

        getService(peripheral, QString("6e400001-b5a3-f393-e0a9-e50e24dcca9e"));

        emit deviceConnected();
        searchedPeripherals.clear();
        close();
    }
}

void BLE_Finder::disconnectDevice(void)
{
    typedef BlexErrorCode (*FunDef)(BlexPeripheral);
    FunDef blexPeripheralDisconnect = (FunDef)libblex->resolve("blexPeripheralDisconnect");

    if (!blexPeripheralDisconnect(peripheral))
    {
        peripheral = nullptr;
        qDebug() << "BLE Disconnected";
        emit deviceDisconnected();
        close();
    }
}

bool BLE_Finder::sendCommand(uint8_t* command)
{
    if (!isBluetoothConnected()) { return false; }

//    typedef void (*callback)(const char* serivce,
//                             const char* characteristic,
//                             const char* data,
//                             size_t data_length);
//    typedef BlexErrorCode (*FunDef)(BlexPeripheral, char*, char*, callback);
//    FunDef blexPeripheralIndicate = (FunDef)libblex->resolve("blexPeripheralIndicate");

    typedef BlexErrorCode (*FuncDef)(BlexPeripheral, char*, char*, size_t, uint8_t*);
    FuncDef blexPeripheralWriteCommand = (FuncDef)libblex->resolve("blexPeripheralWriteCommand");

    for (int j = 0; j < service->characteristic_count; ++j) {
        if (QString(service->characteristics[j].uuid.value) == QString("6e400002-b5a3-f393-e0a9-e50e24dcca9e"))
        {
            qDebug() << service->characteristics[j].uuid.value;

            qDebug() << "BLE Sent status" << blexPeripheralWriteCommand(peripheral, 
                                                                        service->uuid.value,
                                                                        service->characteristics[j].uuid.value,
                                                                        (size_t)sizeof(command),
                                                                        (uint8_t*)command);
        }
        if (QString(service->characteristics[j].uuid.value) == QString("6e400003-b5a3-f393-e0a9-e50e24dcca9e"))
        {
//            qDebug() << "BLE Indicated" << blexPeripheralIndicate(peripheral, service->uuid.value, service->characteristics[j].uuid.value, callbackOnIndicate);
        }
    }

    return true;
}

void BLE_Finder::on_pushButton_connect_clicked(void)
{
    connectDevice();
}

void BLE_Finder::on_pushButton_refresh_clicked(void)
{
    ui->listWidget->clear();
    searchedPeripherals.clear();
    scanDevice();
}


void callbackOnScanStart(BlexAdapter adapter, void* userdata) {}

void callbackOnScanFound(BlexAdapter adapter, BlexPeripheral peripheral, void* userdata)
{
    typedef char* (*FunDef)(BlexPeripheral);
    FunDef blexPeripheralIdentifier = (FunDef)libblex->resolve("blexPeripheralIdentifier");

    if (blexPeripheralIdentifier(peripheral)[0] != 0)
    {
        FunDef blexPeripheralAddress = (FunDef)libblex->resolve("blexPeripheralAddress");
        searchedPeripherals.append(peripheral);
    }
}

void callbackOnIndicate(const char* serivce, 
                        const char* characteristic, 
                        const char* data, 
                        size_t data_length)
{
    if (data[0] == 'S' && data[1] == 'C')
    {
        /* code */
    }
    else if (data[0] == 'E' && data[1] == 'R')
    {
        /* code */
    }
    else if (data[0] == 'E' && data[1] == 'D')
    {
        /* code */
    }
    qDebug() << "responsed!";
}


void Delay_MSec(int msec)   // Non-blocking delay
{
    QEventLoop loop;                                // Define a new event loop
    QTimer::singleShot(msec, &loop, SLOT(quit()));  // Define a timer, connect it to the loop
    loop.exec();                                    // Execute the loop, delay until timeout
}
