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

// Check if US has been polled
bool firstPoll = true;

// Setup function
void setup() {
  // Set to start millis
  astates.start_time = millis();
  astates.last_update_time = millis();
  astates.last_server_time = millis();
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
  if (millis() - astates.last_update_time >= 100 || firstPoll) {
    sensors.ultrasonic_poll(mstates, sstates);
    astates.last_update_time = millis();
    if (firstPoll) {
      firstPoll = false;
      Serial.println("Sneaky first poll completed!");
    }
  }
  if (millis() - astates.last_server_time >= 1000) {
    printCurrentInfo();
    astates.last_server_time = millis();
  }
}

void printCurrentInfo() {
  String data = "L:";
  data += String(mstates.left_speed);
  data += ",R:";
  data += String(mstates.right_speed);
  data += ",D:";
  data += String(sensors.getUltrasonicDistance());
  wifi.messageClient(data);
}
