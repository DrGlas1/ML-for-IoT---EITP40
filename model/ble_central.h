#include <ArduinoBLE.h>

#define NBR_BATCHES_ITER (DYN_NBR_WEIGHTS / BLE_NBR_WEIGHTS)

void do_training();

typedef struct __attribute__( ( packed ) )
{
  int8_t turn;
  uint8_t batch_id;
  float w[BLE_NBR_WEIGHTS];
} ble_data_t;

float* dyn_weights;
ble_data_t bleData;
BLEDevice peripheral;

BLECharacteristic readCharacteristic;
BLECharacteristic writeCharacteristic;


void loopPeripheral() {
  while (peripheral.connected()) {
    if (readCharacteristic.valueUpdated()) {
      readCharacteristic.readValue((byte *)&bleData, sizeof(bleData));

      if (bleData.turn == 1) {
        memcpy(dyn_weights + bleData.batch_id * BLE_NBR_WEIGHTS, bleData.w, BLE_NBR_WEIGHTS * sizeof(bleData.w[0]));

        if (bleData.batch_id == NBR_BATCHES_ITER - 1) {
          do_training();
        }
      }

      if (bleData.turn == CENTRAL_ID && (bleData.batch_id == NBR_BATCHES_ITER - 1 || CENTRAL_ID != 1)) {
        bleData.turn = 0;

        // Should also be sent in batch
        for (int i = 0; i < NBR_BATCHES_ITER; i++) {
          bleData.batch_id = i;
          // Copy weights
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
  readCharacteristic = peripheral.characteristic("19b10001-e8f2-537e-4f6c-d104768a1214");
  writeCharacteristic = peripheral.characteristic("19b10001-e8f2-537e-4f6c-d104768a1215");

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

void setupBLE(float* wbptr) {
  dyn_weights = wbptr;
  BLE.begin();

#if DEBUG
  Serial.println("BLE Central - Weights control Setup Done");
#endif

  // start scanning for peripherals
  BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
}

void loopBLE() {
  peripheral = BLE.available();

  if (peripheral) {

    if (peripheral.localName() != "MLLeader") {
      return;
    }
    BLE.stopScan();

    connectPeripheral();

    BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
  }
}