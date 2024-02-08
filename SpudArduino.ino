// Includes
#include "SensorManager.h"
#include "MotorManager.h"

// Class initialization
SensorManager sensors;
MotorManager motors;

// Data structure of states
sensor_states sstates;
motor_states mstates;

// Setup function
void setup() {
  // Begin serial connection
  Serial.begin(9600);
  // Setup pins
  sensors.pinSetup();
  motors.pinSetup();
}

// Main loop
void loop() {
  // Probe devices passing them states
  // incase changes have occurred
  sensors.probe(sstates, mstates);
  motors.probe(mstates);
  delay(1000);
}
