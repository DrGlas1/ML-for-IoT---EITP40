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

int turn;

void send_iteration_data() {
  turn = 1; // Start over
  bleData.turn = turn;
}

void loopBLE() {
  BLE.poll();
  if (!BLE.central()) return;

  if (writeCharacteristic.written()) {
    int8_t receivedTurn = writeCharacteristic[0];

    // -1 means connection established, send weights, no aggregation since nothing received
    if (receivedTurn == -1) {
      send_iteration_data();
      return;
    }
    Serial.print("Received turn: ");
    Serial.println(receivedTurn);

    // Leader needs to do something
    if (receivedTurn == 0) {
      writeCharacteristic.readValue((byte *)&bleData, sizeof(bleData));
      bleData.turn = ++turn;
      do_training();
    }
  }
}
