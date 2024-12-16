#ifndef BLE_DATA_H
#define BLE_DATA_H

#define READ_UUID "0cc92356-edf6-4828-816e-4cfa94004a84"
#define WRITE_UUID "0cc92356-edf6-4828-816e-4cfa94004a85"

#define NBR_BATCHES_ITER (DYN_NBR_WEIGHTS / BLE_NBR_WEIGHTS)

typedef struct __attribute__( ( packed ) )
{
  int8_t turn;
  uint8_t batch_id;
  float w[BLE_NBR_WEIGHTS];
} ble_data_t;

float* dyn_weights;
ble_data_t bleData;

void do_training();

#endif