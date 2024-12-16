#include <ArduinoBLE.h>
#include "ble.h"

void do_training();
float* dyn_weights;
ble_data_t bleData;

static BLEService weightsService(READ_UUID);
static BLECharacteristic readCharacteristic(READ_UUID, BLERead | BLEIndicate, sizeof(bleData));
static BLECharacteristic writeCharacteristic(WRITE_UUID, BLEWrite, sizeof(bleData));

static void ConnectHandler(BLEDevice central) {
  BLE.advertise();
}

static void DisconnectHandler(BLEDevice central) {
  BLE.advertise();
}

void setupBLE(float* wbptr) {
  dyn_weights = wbptr;
  // initialize the BLE hardware
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1)
      ;
  }
  Serial.println("Setting up CENTRAL BLE");
  do_training(); // leader trains once before setting up

  BLE.setEventHandler(BLEConnected, ConnectHandler);
  BLE.setEventHandler(BLEDisconnected, DisconnectHandler);
  // set advertised local name and service UUID:
  BLE.setLocalName("Leader");
  BLE.setAdvertisedService(weightsService);

  // add the characteristic to the service
  weightsService.addCharacteristic(readCharacteristic);
  weightsService.addCharacteristic(writeCharacteristic);

  // add service
  BLE.addService(weightsService);

  // set the initial value for the characeristic:
  writeCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));
  readCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));

  // start advertising
  BLE.advertise();
}

static void send_data() {
  printWeights(dyn_weights, true);
  for (int i = 0; i < NBR_BATCHES_ITER; i++) {
    bleData.batch_id = i;
    memcpy(bleData.w, dyn_weights + i * BLE_NBR_WEIGHTS, BLE_NBR_WEIGHTS * sizeof(float));
    readCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));
  }
}

inline void store_incoming_weights() {
  memcpy(dyn_weights + bleData.batch_id * BLE_NBR_WEIGHTS, bleData.w, BLE_NBR_WEIGHTS * sizeof(bleData.w[0]));
}

void loopBLE() {
  BLE.poll();
  if (!BLE.central()) {
    return;
  }

  if (writeCharacteristic.written()) {
      writeCharacteristic.readValue((byte *)&bleData, sizeof(bleData));
      if(bleData.batch_id == 255) {
        send_data();
      }
      store_incoming_weights();
      if (bleData.batch_id == NBR_BATCHES_ITER - 1) {
        printWeights(dyn_weights, false);
        send_data();
        do_training();
      }
  }
}
