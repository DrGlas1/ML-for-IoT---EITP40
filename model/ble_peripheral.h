#include <ArduinoBLE.h>
#include "ble.h"

void do_training();

BLEDevice peripheral;
BLECharacteristic readCharacteristic;
BLECharacteristic writeCharacteristic;


void loopPeripheral() {
  while (peripheral.connected()) {
    if (readCharacteristic.valueUpdated()) {
      // Fetch from peripheral
      readCharacteristic.readValue((byte *)&bleData, sizeof(bleData));
      do_training();
      writeCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));
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

  // Inform peripheral, connection is established
  bleData.turn = -1;
  writeCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));

  // Continues until disconnect
  loopPeripheral();

  Serial.println("Peripheral disconnected");
}

void setupBLE() {
  // initialize the BLE hardware
  BLE.begin();

  // start scanning for peripherals
  BLE.scanForUuid(READ_UUID);
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