#include <ArduinoBLE.h>

#include "ble_data.h"

void aggregate_weights();

BLEService weightsService(READ_UUID);
BLECharacteristic readCharacteristic(READ_UUID, BLERead | BLEIndicate, sizeof(bleData));
BLECharacteristic writeCharacteristic(WRITE_UUID, BLEWrite, sizeof(bleData));

inline void store_incoming_weights() {
    memcpy(dyn_weights + bleData.batch_id * BLE_NBR_WEIGHTS, bleData.w,
           BLE_NBR_WEIGHTS * sizeof(bleData.w[0]));
}

void ConnectHandler(BLEDevice central) { BLE.advertise(); }

void DisconnectHandler(BLEDevice central) { BLE.advertise(); }

void setupBLE(float *wbptr) {
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

void send_iteration_data() {
    bleData.turn = CENTRAL_TURN;

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

        if (bleData.turn == SETUP_TURN) {
            send_iteration_data();
            return;
        }

        if (bleData.turn == PERIPHERAL_TURN) {
            store_incoming_weights();

            if (bleData.batch_id == FINAL_ITER) {
                aggregate_weights();
                send_iteration_data();
                do_training();
            }
        }
    }
}
