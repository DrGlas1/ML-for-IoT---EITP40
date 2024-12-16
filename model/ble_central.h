#include <ArduinoBLE.h>
#include "ble.h"

void do_training();
float* dyn_weights;
ble_data_t bleData;

BLEDevice peripheral;
BLECharacteristic readCharacteristic;
BLECharacteristic writeCharacteristic;

void setupBLE(float* wbptr) {
  dyn_weights = wbptr;
  // initialize the BLE hardware
  BLE.begin();

  // start scanning for peripherals
  BLE.scanForUuid(READ_UUID);
}

void send_data() {
  for (int i = 0; i < NBR_BATCHES_ITER; i++) {
    bleData.batch_id = i;
    memcpy(bleData.w, dyn_weights + i * BLE_NBR_WEIGHTS, BLE_NBR_WEIGHTS * sizeof(bleData.w[0]));
    writeCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));
  }
}

void loopPeripheral() {
  while (peripheral.connected()) {
    if (readCharacteristic.valueUpdated()) {
      // Fetch from peripheral
      readCharacteristic.readValue((byte *)&bleData, sizeof(bleData));
      if (bleData.batch_id == NBR_BATCHES_ITER - 1) {
        send_data();
        do_training();
      }
    }
  }
}

void connectPeripheral() {
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
  }

  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
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
  bleData.batch_id = 255;

  // Inform peripheral, connection is established
  writeCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));

  // Continues until disconnect
  loopPeripheral();

  Serial.println("Peripheral disconnected");
}

void loopBLE() {
  // check if a peripheral has been discovered
  peripheral = BLE.available();

  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    if (peripheral.localName() != "Leader") {
      return;
    }

    // stop scanning
    BLE.stopScan();
    connectPeripheral();

    // peripheral disconnected, start scanning again
    BLE.scanForUuid(READ_UUID);
  }
}