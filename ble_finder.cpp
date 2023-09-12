#include "ble_finder.h"
#include "ui_ble_finder.h"

QLibrary* libblex = new QLibrary("libblex.dll");
QList<BlexPeripheral> searchedPeripherals;

float frequency[1024];
float amplitude[1024];
float phase[1024];
quint16 receiveCount = 0;


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

    qDebug() << "Services count:" <<blexPeripheralGetServicesCount(blexPeripheral);

    for (int i = 0; i < blexPeripheralGetServicesCount(blexPeripheral); i++) {
        blexPeripheralGetServices(blexPeripheral, i, blexService);
        if (QString(blexService->uuid.value) == uuid)
        {
            service = blexService;

            return;
        }
    }
}

void BLE_Finder::connectDevice(void)
{
    typedef BlexErrorCode (*FunDef)(BlexPeripheral);
    FunDef blexPeripheralConnect = (FunDef)libblex->resolve("blexPeripheralConnect");

    typedef void (*callback)(const char* serivce,
                             const char* characteristic,
                             const char* data,
                             size_t data_length);
    typedef BlexErrorCode (*FuncDef)(BlexPeripheral, char*, char*, callback);
    FuncDef blexPeripheralNotify = (FuncDef)libblex->resolve("blexPeripheralNotify");

    if (!blexPeripheralConnect(searchedPeripherals[ui->listWidget->currentRow()]))
    {
        peripheral = searchedPeripherals[ui->listWidget->currentRow()];
        getService(peripheral, BLEUART_SERVICE_UUID);

        qDebug() << "BLE Connected";

        for (int j = 0; j < service->characteristic_count; ++j) {
            if (QString(service->characteristics[j].uuid.value) == BLEUART_TX_CHARACTERISTIC_UUID)
            {
                qDebug() << "BLE Notify" << blexPeripheralNotify(peripheral, service->uuid.value, service->characteristics[j].uuid.value, callbackOnIndicate);
            }
        }
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

    typedef BlexErrorCode (*FuncDef)(BlexPeripheral, char*, char*, size_t, uint8_t*);
    FuncDef blexPeripheralWriteCommand = (FuncDef)libblex->resolve("blexPeripheralWriteCommand");

    for (int j = 0; j < service->characteristic_count; ++j) {
        if (QString(service->characteristics[j].uuid.value) == BLEUART_RX_CHARACTERISTIC_UUID)
        {
            if (!blexPeripheralWriteCommand(peripheral,
                                           service->uuid.value,
                                           service->characteristics[j].uuid.value,
                                           (size_t)sizeof(command),
                                           (uint8_t*)command))
            {
                qDebug() << "Data sent successfully";
            }
            else
            {
                qDebug() << "Data sent failed";
                return false;
            }
        }
    }

    if (command[0] == 'S' && command[1] == 'T')
    {
        receiveCount = 0;

        while (receiveCount < freqCount) {
            Delay_MSec(1000);
        }
        qDebug() << "Received: " << receiveCount;
        emit dataReceived(frequency, amplitude, phase, receiveCount);
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

void BLE_Finder::setFreqCount(quint16 newFreqCount)
{
    freqCount = newFreqCount;
}

void callbackOnScanStart(BlexAdapter adapter, void* userdata) { (void)adapter; (void)userdata; }

void callbackOnScanFound(BlexAdapter adapter, BlexPeripheral peripheral, void* userdata)
{
    typedef char* (*FunDef)(BlexPeripheral);
    FunDef blexPeripheralIdentifier = (FunDef)libblex->resolve("blexPeripheralIdentifier");

    if (blexPeripheralIdentifier(peripheral)[0] != 0)
    {
        searchedPeripherals.append(peripheral);
    }

    (void)adapter;
    (void)userdata;
}

void callbackOnIndicate(const char* serivce, 
                        const char* characteristic, 
                        const char* data, 
                        size_t data_length)
{
    switch (data_length) {
    case 12:
        memcpy(&frequency[receiveCount], data, 4);
        memcpy(&amplitude[receiveCount], data+4, 4);
        memcpy(&phase[receiveCount], data+8, 4);
        qDebug() << "Freq: " << frequency[receiveCount] << "Hz";
        qDebug() << "Amplitude: " << amplitude[receiveCount] << "Ohm, Phase: " << phase[receiveCount];

        receiveCount++;

        break;
    default:
        break;
    }

    (void)serivce;
    (void)characteristic;
}

void Delay_MSec(int msec)   // Non-blocking delay
{
    QEventLoop loop;                                // Define a new event loop
    QTimer::singleShot(msec, &loop, SLOT(quit()));  // Define a timer, connect it to the loop
    loop.exec();                                    // Execute the loop, delay until timeout
}
