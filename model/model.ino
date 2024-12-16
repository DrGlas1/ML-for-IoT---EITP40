// Code developed by Nikhil Challa and Simon Erlandsson as part of ML in IOT Course - year : 2022

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

#define DEVICE_TYPE CENTRAL 

#define DYN_NBR_WEIGHTS weights_bias_cnt // MUST BE MULTIPLE OF BLE_NBR_WEIGHTS!
#define BLE_NBR_WEIGHTS 12

// NN parameters
#define LEARNING_RATE 0.01
#define EPOCH 20 

static const int NN_def[] = {first_layer_input_cnt, 20, classes_cnt};

#define DATA_TYPE_FLOAT

// Training and Validation data
#if DEVICE_TYPE == CENTRAL
#include "online_data_5.h"
#else DEVICE_TYPE == PERIPHERAL
#include "online_data_6.h"
#endif
    
#include "NN_functions.h" // Neural Network specific functions and definitions

#if DEVICE_TYPE == CENTRAL
#define NBR_CENTRALS 1 // Config
#include "BLE_peripheral.h"
#elif DEVICE_TYPE == PERIPHERAL
#define CENTRAL_ID 1 // Config
#include "BLE_central.h"
#endif

void destroy() {
  Serial.println("Finished training, shutting down.");
  printAccuracy();
#if ENABLE_BLE
  BLE.stopAdvertise();
  BLE.disconnect();
#endif
  while (1) ;
}

#if DEVICE_TYPE == CENTRAL
void aggregate_weights() {

  Serial.println("Accuracy before aggregation:");
  printAccuracy();
  packUnpackVector(AVERAGE);

  Serial.println("Accuracy after aggregation:");
  printAccuracy();
}
#endif

void do_training() {
  if (iter_cnt >= EPOCH) destroy();

#if DEVICE_TYPE == PERIPHERAL
  packUnpackVector(UNPACK);
  Serial.println("Accuracy using incoming weights:");
  printAccuracy();
#endif

  Serial.print("Epoch count (training count): ");
  Serial.print(++iter_cnt);
  Serial.println();

  // reordering the index for more randomness and faster learning
  shuffleIndx();
  
  // starting forward + Backward propagation
  for (int j = 0;j < numTrainData;j++) {
    generateTrainVectors(j);  
    forwardProp();
    backwardProp();
  }

  // pack the vector for bluetooth transmission
  forwardProp();
#if DEVICE_TYPE == PERIPHERAL
  packUnpackVector(PACK);
  Serial.println("Accuracy after local training:");
#endif
}

void setup() {
  // randomly initialize the seed based on time
  srand(time(0));

  Serial.begin(9600);
  delay(5000);

  // We need to count how many weights and bias we need to transfer
  // the code is only for Fully connected layers
  weights_bias_cnt = calcTotalWeightsBias();

  // weights_bias_cnt has to be multiple of BLE_NBR_WEIGHTS
  int remainder = weights_bias_cnt % BLE_NBR_WEIGHTS;
  if (remainder != 0)
    weights_bias_cnt += BLE_NBR_WEIGHTS - remainder;

  Serial.print("The total number of weights and bias used for BLE: ");
  Serial.println(weights_bias_cnt);

  // Allocate common weight vector, and pass to setupNN, setupBLE
  DATA_TYPE* WeightBiasPtr = (DATA_TYPE*) calloc(weights_bias_cnt, sizeof(DATA_TYPE));
  Serial.println(weights_bias_cnt);
  Serial.println(12);
  setupNN(WeightBiasPtr);
  printAccuracy();
  setupBLE(WeightBiasPtr);
}

void loop() {
  loopBLE();
  do_training(); // Local training
}