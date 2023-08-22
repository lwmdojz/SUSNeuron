#ifndef BLE_FINDER_H
#define BLE_FINDER_H

#include <QLibrary>
#include <QWidget>
#include <QTCore>
#include <QTimer>

#define SIMPLEBLE_UUID_STR_LEN 37  // 36 characters + null terminator
#define SIMPLEBLE_CHARACTERISTIC_MAX_COUNT 16
#define SIMPLEBLE_DESCRIPTOR_MAX_COUNT 16

// TODO: Add proper error codes.
typedef enum {
    SIMPLEBLE_SUCCESS = 0,
    SIMPLEBLE_FAILURE = 1,
} BlexErrorCode;

typedef struct {
    char value[SIMPLEBLE_UUID_STR_LEN];
} BlexUUID;

typedef struct {
    BlexUUID uuid;
} simpleble_descriptor_t;

typedef struct {
    BlexUUID uuid;
    bool can_read;
    bool can_write_request;
    bool can_write_command;
    bool can_notify;
    bool can_indicate;
    size_t descriptor_count;
    simpleble_descriptor_t descriptors[SIMPLEBLE_DESCRIPTOR_MAX_COUNT];
} simpleble_characteristic_t;

typedef struct {
    BlexUUID uuid;
    size_t data_length;
    uint8_t data[27];
    // Note: The maximum length of a BLE advertisement is 31 bytes.
    // The first byte will be the length of the field,
    // the second byte will be the type of the field,
    // the next two bytes will be the service UUID,
    // and the remaining 27 bytes are the manufacturer data.
    size_t characteristic_count;
    simpleble_characteristic_t characteristics[SIMPLEBLE_CHARACTERISTIC_MAX_COUNT];
} BlexService;

typedef struct {
    uint16_t manufacturer_id;
    size_t data_length;
    uint8_t data[27];
    // Note: The maximum length of a BLE advertisement is 31 bytes.
    // The first byte will be the length of the field,
    // the second byte will be the type of the field (0xFF for manufacturer data),
    // the next two bytes will be the manufacturer ID,
    // and the remaining 27 bytes are the manufacturer data.
} BlexManufacturerData;

typedef void* BlexAdapter;
typedef void* BlexPeripheral;

typedef enum {
    SIMPLEBLE_OS_WINDOWS = 0,
    SIMPLEBLE_OS_MACOS = 1,
    SIMPLEBLE_OS_LINUX = 2,
} simpleble_os_t;

typedef enum {
    SIMPLEBLE_ADDRESS_TYPE_PUBLIC = 0,
    SIMPLEBLE_ADDRESS_TYPE_RANDOM = 1,
    SIMPLEBLE_ADDRESS_TYPE_UNSPECIFIED = 2,
} BlexAddressType;


namespace Ui {
class BLE_Finder;
}

class BLE_Finder : public QWidget
{
    Q_OBJECT

public:
    explicit BLE_Finder(QWidget *parent = nullptr);
    ~BLE_Finder();

    bool isBluetoothEnabled(void);
    bool isBluetoothConnected(void);

    void scanDevice(void);
    void connectDevice(void);
    void disconnectDevice(void);

    void getService(BlexPeripheral peripheral, QString uuid);

//    bool sendCommand(char *data, size_t length);
    bool sendCommand(uint8_t* command);


private slots:
    void on_pushButton_connect_clicked();

    void on_pushButton_refresh_clicked();

signals:
    void deviceConnected(void);
    void deviceDisconnected(void);

private:
    Ui::BLE_Finder *ui;

    BlexAdapter     adapter;
    BlexPeripheral  peripheral;
    BlexService*    service;

    uint8_t         data;

};

void Delay_MSec(int msec);

void callbackOnScanStart(BlexAdapter adapter, void* userdata);
void callbackOnScanFound(BlexAdapter adapter, BlexPeripheral peripheral, void* userdata);

void callbackOnIndicate(const char* serivce, const char* characteristic, const char* data, size_t data_length);



#endif // BLE_FINDER_H
