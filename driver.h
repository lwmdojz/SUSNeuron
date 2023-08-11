#include <stdio.h>
#include <stdlib.h>

#include "simpleble_c/simpleble.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define BLEX_EXPORT __declspec(dllexport)
#else
#define BLEX_EXPORT
#endif

// proxy func.
#ifdef __cplusplus
extern "C" {
#endif
BLEX_EXPORT typedef simpleble_err_t BlexErrorCode;
BLEX_EXPORT typedef simpleble_adapter_t BlexAdapter;
BLEX_EXPORT typedef simpleble_peripheral_t BlexPeripheral;
BLEX_EXPORT typedef simpleble_address_type_t BlexAddressType;
BLEX_EXPORT typedef simpleble_service_t BlexService;
BLEX_EXPORT typedef simpleble_manufacturer_data_t BlexManufacturerData;
BLEX_EXPORT typedef simpleble_uuid_t BlexUUID;

// adapter.h

BLEX_EXPORT bool    blexAdapterIsBluetoothEnabled();

BLEX_EXPORT size_t  blexAdapterGetCount();

BLEX_EXPORT BlexAdapter blexAdapterGetHandle(size_t index);

BLEX_EXPORT void    blexAdapterReleaseHandle(BlexAdapter handle);

BLEX_EXPORT char*   blexAdapterIdentifier(BlexAdapter handle);

BLEX_EXPORT char*   blexAdapterAddress(BlexAdapter handle);

BLEX_EXPORT BlexErrorCode blexAdapterScanStart(BlexAdapter handle); //1

BLEX_EXPORT BlexErrorCode blexAdapterScanStop(BlexAdapter handle); //1

BLEX_EXPORT BlexErrorCode blexAdapterScanIsActive(BlexAdapter handle, bool * active);

BLEX_EXPORT BlexErrorCode blexAdapterScanFor(BlexAdapter handle, int timeout_ms);

BLEX_EXPORT size_t    blexAdapterScanGetResultsCount(BlexAdapter handle);

BLEX_EXPORT BlexPeripheral blexAdapterScanGetResultsHandle(BlexAdapter handle,
                                                    size_t index);

BLEX_EXPORT size_t    blexAdapterGetPairedPeripheralsCount(BlexAdapter handle);

BLEX_EXPORT BlexPeripheral blexAdapterGetPairedPeripheralsHandle(BlexAdapter handle,
                                                          size_t index);

// register when start
BLEX_EXPORT BlexErrorCode blexAdapterSetCallbackOnScanStart(
      BlexAdapter handle,
      void (*callback)(BlexAdapter adapter, void* userdata),
      void* userdata
    );

BLEX_EXPORT BlexErrorCode blexAdapterSetCallbackOnScanStop(
      BlexAdapter handle,
      void (*callback)(BlexAdapter adapter, void* userdata),
      void* userdata
    );

BLEX_EXPORT BlexErrorCode blexAdapterSetCallbackOnScanUpdated(
    BlexAdapter handle,
    void (*callback)(BlexAdapter adapter, BlexPeripheral peripheral, void* userdata),
    void* userdata
    );

BLEX_EXPORT BlexErrorCode blexAdapterSetCallbackOnScanFound(
    BlexAdapter handle,
    void (*callback)(BlexAdapter adapter, BlexPeripheral peripheral, void* userdata),
    void* userdata);

// peripheral.h
/****** get peripheral information *******/
BLEX_EXPORT void blexPeripheralReleaseHandle(BlexPeripheral handle);

BLEX_EXPORT char* blexPeripheralIdentifier(BlexPeripheral handle);

BLEX_EXPORT char* blexPeripheralAddress(BlexPeripheral handle);

BLEX_EXPORT BlexAddressType blexPeripheralAddressType(BlexPeripheral handle);

BLEX_EXPORT int16_t   blexPeripheralRssi(BlexPeripheral handle);

BLEX_EXPORT int16_t   blexPeripheralPower(BlexPeripheral handle);

BLEX_EXPORT uint16_t  blexPeripheralMtu(BlexPeripheral handle);

BLEX_EXPORT BlexErrorCode blexPeripheralConnect(BlexPeripheral handle);

BLEX_EXPORT BlexErrorCode blexPeripheralDisconnect(BlexPeripheral handle);

BLEX_EXPORT BlexErrorCode blexPeripheralIsConnected(BlexPeripheral handle, bool* connected);

BLEX_EXPORT BlexErrorCode blexPeripheralIsConnectable(BlexPeripheral handle, bool* connectable);

BLEX_EXPORT BlexErrorCode blexPeripheralIsPaired(BlexPeripheral handle, bool* paired);

BLEX_EXPORT BlexErrorCode blexPeripheralUnpair(BlexPeripheral handle);

BLEX_EXPORT size_t     blexPeripheralGetServicesCount(BlexPeripheral handle);

BLEX_EXPORT BlexErrorCode blexPeripheralGetServices(BlexPeripheral handle,
                                                size_t index,
                                          BlexService* services);

BLEX_EXPORT size_t     blexPeripheralManufacturerDataCount(BlexPeripheral handle);

BLEX_EXPORT BlexErrorCode blexPeripheralManufacturerDataGet(
    BlexPeripheral handle, size_t index, BlexManufacturerData* manufacturer_data
  );

BLEX_EXPORT BlexErrorCode blexPeripheralRead(BlexPeripheral handle,
                                       char* service,
                                       char* characteristic,
                                     size_t* data_length,
                                   uint8_t** data);

BLEX_EXPORT BlexErrorCode blexPeripheralWriteRequest(BlexPeripheral handle,
                                                char* service,
                                                char* characteristic,
                                                  size_t data_length,
                                                uint8_t* data);

BLEX_EXPORT BlexErrorCode blexPeripheralWriteCommand(
                                             BlexPeripheral handle,
                                              char* service,
                                              char* characteristic,
                                             size_t data_length,
                                              uint8_t* data);

BLEX_EXPORT BlexErrorCode blexPeripheralNotify(BlexPeripheral handle,
                                              char* service,
                                              char* characteristic,
                                              void (*callback)(const char* service, const char* characteristic, const uint8_t* data, size_t data_length)
                                              );

BLEX_EXPORT BlexErrorCode blexPeripheralIndicate(BlexPeripheral handle,
                                              char* service,
                                              char* characteristic,
                                              void (*callback)(const char* service, const char* characteristic, const uint8_t* data, size_t data_length)
                                              );

BLEX_EXPORT BlexErrorCode blexPeripheralUnsubscribe(BlexPeripheral handle,
                                              char* service,
                                              char* characteristic);

BLEX_EXPORT BlexErrorCode blexPeripheralReadDescriptor(BlexPeripheral handle,
                                                  char* service,
                                                  char* characteristic,
                                                  char* descriptor,
                                                  size_t* data_length,
                                                uint8_t** data) {
  BlexUUID service_ = to_uuid(service), characteristic_ = to_uuid(characteristic), descriptor_ = to_uuid(descriptor);

BLEX_EXPORT BlexErrorCode blexPeripheralWriteDescriptor(BlexPeripheral handle,
                                                  char*  service,
                                                  char*  characteristic,
                                                  char*  descriptor,
                                                  size_t data_length,
                                            const uint8_t* data);

BLEX_EXPORT BlexErrorCode blexPeripheralSetCallbackOnConnected(BlexPeripheral handle,
                                                            void (*callback)(BlexPeripheral peripheral, void* userdata),
                                                            void* userdata);

BLEX_EXPORT BlexErrorCode blexPeripheralSetCallbackOnDisconnected(BlexPeripheral handle,
                                                               void (*callback)(BlexPeripheral peripheral, void* userdata),
                                                               void* userdata);
#ifdef __cplusplus
}
#endif
