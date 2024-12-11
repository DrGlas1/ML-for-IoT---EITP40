#include <ArduinoBLE.h>
#include "ble.h"

void do_training();

BLEService weightsService(READ_UUID);
BLECharacteristic readCharacteristic(READ_UUID, BLERead | BLEIndicate, sizeof(bleData));
BLECharacteristic writeCharacteristic(WRITE_UUID, BLEWrite, sizeof(bleData));

void ConnectHandler(BLEDevice central) {
  BLE.advertise();
}

void DisconnectHandler(BLEDevice central) {
  BLE.advertise();
}

void setupBLE() {
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

void loopBLE() {
  BLE.poll();
  if (!BLE.central()) {
    return;
  }

  if (writeCharacteristic.written()) {
      writeCharacteristic.readValue((byte *)&bleData, sizeof(bleData));
      do_training();
      readCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));
  }
}
