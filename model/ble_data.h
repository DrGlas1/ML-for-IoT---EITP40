#ifndef BLE_DATA_H
#define BLE_DATA_H

#define READ_UUID "19b10001-e8f2-537e-4f6c-d104768a1214"
#define WRITE_UUID "19b10001-e8f2-537e-4f6c-d104768a1215"

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