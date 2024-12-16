#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<TinyMLShield.h>

#define LEARNING_RATE 0.1    // The learning rate used to train your network
#define EPOCH 10             // The maximum number of epochs
#define DATA_TYPE_FlOAT      // The data type used: Set this to DATA_TYPE_DOUBLE for higher precision
#define CENTRAL

extern const int first_layer_input_cnt;
extern const int classes_cnt;

static const unsigned int NN_def[] = {first_layer_input_cnt, 20, classes_cnt};

#ifdef CENTRAL
#include "ble_central.h"
#else
#include "ble_peripheral.h"
#endif

bool running = true;
int iter_cnt = 0;           // This keeps track of the number of epochs you've trained on the Arduino
#define DEBUG 1             // This prints the weights of your network in case you want to do debugging (set to 1 if you want to see that)


void aggregate_weights() {

  Serial.println("Accuracy before aggregation:");
  printAccuracy();

  packUnpackVector(AVERAGE);

  Serial.println("Accuracy after aggregation:");
  printAccuracy();
}

// This function contains your training loop
void do_training() {
  // Print the epoch number
  Serial.print("Epoch count (training count): ");
  Serial.print(++iter_cnt);
  Serial.println();

  if(iter_cnt >= EPOCH) running = false;

  // reordering the index for more randomness and faster learning
  shuffleIndx();

  // starting forward + Backward propagation
  for (int j = 0;j < numTrainData;j++) {
    generateTrainVectors(j);
    forwardProp();
    backwardProp();
  }
  packUnpackVector(PACK);
  aggregate_weights();
}


void setup() {
  srand(0);

  Serial.begin(9600);
  delay(5000);
  while (!Serial);

  // Initialize the TinyML Shield
  initializeShield();

  // Calculate how many weights and biases we're training on the device.
  int weights_bias_cnt = calcTotalWeightsBias();

  Serial.print("The total number of weights and bias used for on-device training on Arduino: ");
  Serial.println(weights_bias_cnt);

  // Allocate common weight vector, and pass to setupNN, setupBLE
  DATA_TYPE* WeightBiasPtr = (DATA_TYPE*) calloc(weights_bias_cnt, sizeof(DATA_TYPE));

  setupNN(WeightBiasPtr);  // CREATES THE NETWORK BASED ON NN_def[]
  Serial.print("The accuracy before training");
  printAccuracy();
  setupBLE(WeightBiasPtr);
}

void loop() {
  if(running) {
    loopBLE();
  }
}
