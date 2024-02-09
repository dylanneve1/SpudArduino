// Includes
#include "SensorManager.h"
#include "MotorManager.h"
#include "SpudArduino.h"

// Class initialization
SensorManager sensors;
MotorManager motors;

// Data structure of states
sensor_states sstates;
motor_states mstates;
arduino_states astates;

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
  astates.time += 1;
  delay(DELAY_TIME);
}
