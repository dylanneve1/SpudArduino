// Includes
#include "SensorManager.h"
#include "SpudArduino.h"

// Class initialization
SensorManager sensors;

// Data structure of states
sensor_states sstates;

// Setup function
void setup() {
  // Begin serial connection
  Serial.begin(9600);
  // Setup pins
  sensors.pinSetup();
}

// Main loop
void loop() {
  
}
