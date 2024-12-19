#include <ArduinoBLE.h>

#include "ble_data.h"

void aggregate_weights();

BLEService weightsService(READ_UUID);
BLECharacteristic readCharacteristic(READ_UUID, BLERead | BLEIndicate, sizeof(bleData));
BLECharacteristic writeCharacteristic(WRITE_UUID, BLEWrite, sizeof(bleData));

void store_incoming_weights() {
    memcpy(dyn_weights + bleData.batch_id * BLE_NBR_WEIGHTS, bleData.w,
           BLE_NBR_WEIGHTS * sizeof(bleData.w[0]));
}

void ConnectHandler(BLEDevice central) { BLE.advertise(); }

void DisconnectHandler(BLEDevice central) { BLE.advertise(); }

void setupBLE(float *wbptr) {
    dyn_weights = wbptr;

    if (!BLE.begin()) {
        Serial.println("starting BLE failed!");
        while (1);
    }
    BLE.setEventHandler(BLEConnected, ConnectHandler);
    BLE.setEventHandler(BLEDisconnected, DisconnectHandler);

    BLE.setLocalName("MLLeader");
    BLE.setAdvertisedService(weightsService);

    weightsService.addCharacteristic(readCharacteristic);
    weightsService.addCharacteristic(writeCharacteristic);

    BLE.addService(weightsService);

    writeCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));
    readCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));

    BLE.advertise();

    do_training();
}

void send_iteration_data() {
    bleData.turn = RUN;

    for (int i = 0; i < NBR_BATCHES_ITER; i++) {
        bleData.batch_id = i;
        memcpy(bleData.w, dyn_weights + i * BLE_NBR_WEIGHTS,
               BLE_NBR_WEIGHTS * sizeof(bleData.w[0]));
        readCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));
    }
}

void loopBLE() {
    BLE.poll();
    if (!BLE.central()) return;

    if (writeCharacteristic.written()) {
        writeCharacteristic.readValue((byte *)&bleData, sizeof(bleData));

        if (bleData.turn == SETUP) {
            do_training();
            send_iteration_data();
            return;
        }

        if (bleData.turn == RUN) {
            store_incoming_weights();

            if (bleData.batch_id == FINAL_ITER) {
                aggregate_weights();
                do_training();
                send_iteration_data();
            }
        }
    }
}
