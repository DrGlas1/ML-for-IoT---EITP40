#include "data.h"
#include "NN_functions.h"

#define WRITE_UUID "19B10001-E8F2-537E-4F6C-D104768A1215"
#define READ_UUID "19b10001-e8f2-537e-4f6c-d104768a1214"
// The BLE can only send a limited number of bytes per transmission
#define BLE_NBR_WEIGHTS 12
#define DYN_NBR_WEIGHTS calcTotalWeightsBias()
#define NBR_BATCHES_ITER (DYN_NBR_WEIGHTS / BLE_NBR_WEIGHTS)

typedef struct __attribute__( ( packed ) )
{
  uint8_t batch_id;
  float w[BLE_NBR_WEIGHTS];
} ble_data_t;

ble_data_t bleData;