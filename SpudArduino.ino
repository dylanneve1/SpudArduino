// Includes
#include "SensorManager.h"
#include "MotorManager.h"
#include "SpudArduino.h"
#include "GuidanceManager.h"
#include "WiFiManager.h"

// Class initialization
SensorManager sensors;
MotorManager motors;
GuidanceManager guidance;
WiFiManager wifi;

// Data structure of states
sensor_states sstates;
motor_states mstates;
arduino_states astates;

// Setup function
void setup() {
  // Set to start millis
  astates.start_time = millis();
  astates.last_update_time = millis();
  // Begin serial connection
  Serial.begin(9600);
  // Setup pins
  sensors.pinSetup();
  motors.pinSetup();
  // Wireless Setup
  wifi.setupAP();
  wifi.setupServer();
}

// Main loop
void loop() {
  String message = "Hello from Arduino...";
  // Send test message
  wifi.messageClient(message);
  // Refresh the current hits
  guidance.refresh(mstates);
  // Serial.print("Current hits in last ");
  // Serial.print(HIT_TIMEFRAME);
  // Serial.print(" is ");
  // Serial.println(guidance.poll());
  // Set the current time before starting loop
  astates.current_time = millis();
  // Sensors
  sensors.probe(sstates, mstates, guidance);
  motors.probe(mstates);
  if (millis() - astates.last_update_time >= 100) {
    sensors.ultrasonic_poll(mstates, sstates);
    astates.last_update_time = millis();
  }
}
