#include <ArduinoBLE.h>
#include "ble_data.h"

BLEDevice peripheral;

BLECharacteristic readCharacteristic;
BLECharacteristic writeCharacteristic;


void loopPeripheral() {
  while (peripheral.connected()) {
    if (readCharacteristic.valueUpdated()) {
      readCharacteristic.readValue((byte *)&bleData, sizeof(bleData));

      if (bleData.turn == CENTRAL_TURN) {
        memcpy(dyn_weights + bleData.batch_id * BLE_NBR_WEIGHTS, bleData.w, BLE_NBR_WEIGHTS * sizeof(bleData.w[0]));

        if (bleData.batch_id == FINAL_ITER) {
          do_training();
        }
      }

      if (bleData.turn == CENTRAL_ID && bleData.batch_id == FINAL_ITER) {
        bleData.turn = PERIPHERAL_TURN;

        for (int i = 0; i < NBR_BATCHES_ITER; i++) {
          bleData.batch_id = i;
          memcpy(bleData.w, dyn_weights + i * BLE_NBR_WEIGHTS, BLE_NBR_WEIGHTS * sizeof(bleData.w[0]));
          writeCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));
        }
      }
    }
  }
}

void connectPeripheral() {
  if (peripheral.connect()) {
  } else {
    return;
  }

  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
  } else {
    peripheral.disconnect();
    return;
  }

  // retrieve the Weights characteristics
  readCharacteristic = peripheral.characteristic(READ_UUID);
  writeCharacteristic = peripheral.characteristic(WRITE_UUID);

  if (!readCharacteristic || !writeCharacteristic) {
    peripheral.disconnect();
    return;
  }
  if (!readCharacteristic.subscribe()) {
    peripheral.disconnect();
    return;
  }

  // Inform peripheral, connection is established
  bleData.turn = SETUP_TURN;
  writeCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));

  // Continues until disconnect
  loopPeripheral();

  Serial.println("Peripheral disconnected");
}

void setupBLE(float* wbptr) {
  dyn_weights = wbptr;
  BLE.begin();

  // start scanning for peripherals
  BLE.scanForUuid(READ_UUID);
}

void loopBLE() {
  peripheral = BLE.available();

  if (peripheral) {

    if (peripheral.localName() != "MLLeader") {
      return;
    }
    BLE.stopScan();

    connectPeripheral();

    BLE.scanForUuid(READ_UUID);
  }
}