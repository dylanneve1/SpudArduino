// Includes
#include "SensorManager.h"
#include "MotorManager.h"

// Sensors class initialization
SensorManager sensors;
MotorManager motors;

// Data structure of sensor states
sensor_states sstates;
motor_states mstates;

// Setup function
void setup() {
  // Begin serial connection
  Serial.begin(9600);
  // Setup pins for sensors
  sensors.pinSetup();
  motors.pinSetup();
}

// Main loop
void loop() {
  // Probe sensors passing them states
  // incase changes have occurred
  sensors.probe(sstates, mstates);
  motors.probe(mstates);
  delay(1000);
}
