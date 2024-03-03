// Includes
#include "SensorManager.h"
#include "SpudArduino.h"
#include "VisionManager.h"

HUSKYLENS lens;

// Class initialization
VisionManager vision(lens);

// Setup function
void setup() {
  // Begin serial connection
  Serial.begin(9600);
  // Setup HuskyLens
  if(!vision.VisionSetup()) {
    Serial.println("SPUDARDUINO: Fatal error setting up VisionManager");
  } else {
    Serial.println("SPUDARDUINO: Setup complete!");
  }
}

// Main loop
void loop() {
  vision.FollowTrainedObject();
}
