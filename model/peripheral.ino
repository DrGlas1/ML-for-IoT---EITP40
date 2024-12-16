/*#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<TinyMLShield.h>

#define LEARNING_RATE 0.1    // The learning rate used to train your network
#define EPOCH 10             // The maximum number of epochs
#define DATA_TYPE_FLOAT      // The data type used: Set this to DATA_TYPE_DOUBLE for higher precision
#define PERIPHERAL 1
#define BATCHES (1562 / arraySize)
#define BATCH_SIZE 22        // Same as BATCH_SIZE in ble.h

extern const int first_layer_input_cnt;
extern const int classes_cnt;

static const unsigned int NN_def[] = {first_layer_input_cnt, 20, classes_cnt};

#include "data.h"       // The data, labels and the sizes of all objects are stored here
#include "NN_functions.h"   // All NN functions are stored here
#include "ble_peripheral.h"

bool running = true;
int iter_cnt = 0;           // This keeps track of the number of epochs you've trained on the Arduino
#define DEBUG 0             // This prints the weights of your network in case you want to do debugging (set to 1 if you want to see that)


// This function contains your training loop
void do_training() {

  // Print the weights if you want to debug
#if DEBUG
  Serial.println("Now Training");
  PRINT_WEIGHTS();
#endif
  WeightBiasPtr = bleData.w
  packUnpackVector(1)
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

  Serial.println("Accuracy after local training:");
  printAccuracy();

  
}

void copy_weights_to_BLE() {
  for (int i = 0; i < BATCHES; i++) {
      bleData.batch_id = i;
      memcpy(bleData.w, WeightBiasPtr + i * arraySize, arraySize * sizeof(bleData.w[0]));
      readCharacteristic.writeValue((byte *)&bleData, sizeof(bleData));
  }
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
  Serial.println(sizeof(int));  // Will print 2 or 4 depending on the platform

  Serial.print("The total number of weights and bias used for on-device training on Arduino: ");
  Serial.println(weights_bias_cnt);

  // Allocate common weight vector, and pass to setupNN, setupBLE
  DATA_TYPE* WeightBiasPtr = (DATA_TYPE*) calloc(weights_bias_cnt, sizeof(DATA_TYPE));

  setupNN(WeightBiasPtr);  // CREATES THE NETWORK BASED ON NN_def[]
  Serial.print("The accuracy before training");
  printAccuracy();
  setupBLE();
}

void loop() {
  if(running) {
    loopBLE();
  }
}
*/