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
  // Set to start millis
  astates.start_time = millis();
  // Begin serial connection
  Serial.begin(9600);
  // Setup pins
  sensors.pinSetup();
  motors.pinSetup();
}

// Main loop
void loop() {
  // Set the current time before starting loop
  astates.current_time = millis();
  // Probe devices passing them states
  // incase changes have occurred
  sensors.probe(sstates, mstates);
  motors.probe(mstates);
  delay(DELAY_TIME);
}
