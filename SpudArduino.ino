// Includes
#include "SensorManager.h"
#include "SpudArduino.h"
#include "VisionManager.h"

// HuskyLens
HUSKYLENS* lens;
sensor_states* sstates;

// Class initialization
SensorManager* sensors = new SensorManager(sstates);
VisionManager* vision = new VisionManager(lens, sensors);

// Setup function
void setup() {
  // Begin serial connection
  Serial.begin(9600);
  // Setup pins
  sensors->pinSetup();
  // Setup HuskyLens
  if(!vision->VisionSetup()) {
    Serial.println("SPUDARDUINO: Fatal error setting up VisionManager");
  } else {
    Serial.println("SPUDARDUINO: Setup complete!");
  }
}

// Main loop
void loop() {
  vision->FollowTrainedObject();
}
