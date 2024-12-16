#include <ArduinoBLE.h>
#include "ble_data.h"

void aggregate_weights();

BLEService weightsService(READ_UUID);
BLECharacteristic readCharacteristic(READ_UUID, BLERead | BLEIndicate, sizeof(bleData));
BLECharacteristic writeCharacteristic(WRITE_UUID, BLEWrite, sizeof(bleData));


inline void store_incoming_weights() {
  memcpy(dyn_weights + bleData.batch_id * BLE_NBR_WEIGHTS, bleData.w, BLE_NBR_WEIGHTS * sizeof(bleData.w[0]));

  if (bleData.batch_id == NBR_BATCHES_ITER - 1) {
    aggregate_weights();
  }
}

void ConnectHandler(BLEDevice central) {
  BLE.advertise();
}

void DisconnectHandler(BLEDevice central) {
  BLE.advertise();
}

void setupBLE(float* wbptr) {
  dyn_weights = wbptr;
  // initialize the BLE hardware
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }
  BLE.setEventHandler(BLEConnected, ConnectHandler);
  BLE.setEventHandler(BLEDisconnected, DisconnectHandler);
  // set advertised local name and service UUID:
  BLE.setLocalName("MLLeader");
  BLE.setAdvertisedService(weightsService);

  // add the characteristic to the service
  weightsService.addCharacteristic(readCharacteristic);
  weightsService.addCharacteristic(writeCharacteristic);

  // add service
  BLE.addService(weightsService);

  // set the initial value for the characeristic:
  writeCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));
  readCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));

  BLE.advertise();

  do_training();
}

int turn;

void send_iteration_data() {
  turn = 1;
  bleData.turn = turn;

  for (int i = 0; i < NBR_BATCHES_ITER; i++) {
    bleData.batch_id = i;
    memcpy(bleData.w, dyn_weights + i * BLE_NBR_WEIGHTS, BLE_NBR_WEIGHTS * sizeof(bleData.w[0]));
    readCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));
  }
}

void loopBLE() {
  BLE.poll();
  if (!BLE.central()) return;

  if (writeCharacteristic.written()) {
    int8_t receivedTurn = writeCharacteristic[0];

    if (receivedTurn == -1) {
      send_iteration_data();
      return;
    }

    if (receivedTurn == 0) {
      uint8_t batch_id = writeCharacteristic[1];


      writeCharacteristic.readValue((byte *)&bleData, sizeof(bleData));
      store_incoming_weights();

      if (turn == NBR_CENTRALS && batch_id == NBR_BATCHES_ITER - 1) {
        send_iteration_data();
        do_training();
        
      } else if (bleData.batch_id == NBR_BATCHES_ITER - 1 ) {
        bleData.turn = ++turn;
        readCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));
      }
    }
  }
}