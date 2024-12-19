#define CENTRAL 1
#define PERIPHERAL 2

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int iter_cnt = 0;
int weights_bias_cnt = 0;
extern const int first_layer_input_cnt;
extern const int classes_cnt;

#define DEVICE_TYPE CENTRAL // Which device is being exported: CENTRAL or PERIPHERAL?

#define DYN_NBR_WEIGHTS weights_bias_cnt // MUST BE MULTIPLE OF BLE_NBR_WEIGHTS!
#define BLE_NBR_WEIGHTS 12

#define LEARNING_RATE 0.01
#define EPOCH 20 

static const int NN_def[] = {first_layer_input_cnt, 20, classes_cnt};

#if DEVICE_TYPE == PERIPHERAL
#include "la_ferrari_and_optimus.h"
#else
#include "cybertruck_and_optimus.h"
#endif
    
#include "NN_functions.h"

#if DEVICE_TYPE == PERIPHERAL
#include "BLE_peripheral.h"
#else
#include "BLE_central.h"
#endif

void destroy() {
  Serial.println("Finished training, shutting down.");
  printAccuracy();
  BLE.stopAdvertise();
  BLE.disconnect();
  while (1) ;
}

#if DEVICE_TYPE == PERIPHERAL
void aggregate_weights() {
  Serial.println("BEFORE AGGREGATION");
  printAccuracy();

  packUnpackVector(AVERAGE);

  Serial.println("AFTER AGGREGATION");
  printAccuracy();
}
#endif


void do_training() {
#if DEVICE_TYPE == PERIPHERAL
  if (iter_cnt >= EPOCH) destroy();
#endif

#if DEVICE_TYPE == CENTRAL
  packUnpackVector(UNPACK);
  Serial.println("Accuracy using incoming weights:");
  printAccuracy();
#endif

  Serial.print("EPOCH: ");
  Serial.print(++iter_cnt);
  Serial.println();

  shuffleIndx();

  for (int j = 0;j < numTrainData;j++) {
    generateTrainVectors(j);  
    forwardProp();
    backwardProp();
  }

  forwardProp();
#if DEVICE_TYPE == CENTRAL
  packUnpackVector(PACK);
  Serial.println("Accuracy after local training:");
  printAccuracy();
#endif
}

void setup() {
  srand(time(0));

  Serial.begin(9600);
  delay(5000);

  weights_bias_cnt = calcTotalWeightsBias();


  int remainder = weights_bias_cnt % BLE_NBR_WEIGHTS;
  if (remainder != 0)
    weights_bias_cnt += BLE_NBR_WEIGHTS - remainder;

  Serial.print("The total number of weights and bias used for BLE: ");
  Serial.println(weights_bias_cnt);

  float* WeightBiasPtr = (float*) calloc(weights_bias_cnt, sizeof(float));

  setupNN(WeightBiasPtr);
  printAccuracy();
  setupBLE(WeightBiasPtr);
}

void loop() {
  loopBLE();
}
