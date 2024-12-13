#include "data.h"
#include "NN_functions.h"

#define WRITE_UUID "19B10001-E8F2-537E-4F6C-D104768A1215"
#define READ_UUID "19b10001-e8f2-537e-4f6c-d104768a1214"
// The BLE can only send a limited number of bytes per transmission
#define BLE_NBR_WEIGHTS 12
#define DYN_NBR_WEIGHTS calcTotalWeightsBias()
#define NBR_BATCHES_ITER (DYN_NBR_WEIGHTS / BLE_NBR_WEIGHTS)
#define DEBUG_BLE 0

typedef struct __attribute__( ( packed ) )
{
  uint8_t batch_id;
  float w[BLE_NBR_WEIGHTS];
} ble_data_t;

void printBLE(ble_data_t bleData, bool sending) {
#if DEBUG_BLE
    if(sending) Serial.println("Sending values: ");
    else Serial.println("Received values: ");
    Serial.print("Batch id: ");
    Serial.println(bleData.batch_id);
    Serial.print("Weights: ");
    for (int i = 0; i < BLE_NBR_WEIGHTS; i++) {
        Serial.print(bleData.w[i]);
        Serial.print(" ");
    }
    Serial.println();
#endif
}

void printWeights(float* dyn_weights, bool sending) {
#if DEBUG_BLE
  if(sending) Serial.print("Sending values: ");
  else Serial.print("Received values: ");
  for (int i = 0; i < DYN_NBR_WEIGHTS; i++) {
      Serial.print(dyn_weights[i]);
      Serial.print(" ");
  }
  Serial.println();
#endif
} 