// Includes
#include "SensorManager.h"
#include "SpudArduino.h"
#include "WiFiManager.h"

// Class initialization
SensorManager sensors;
WiFiManager wifi;

// Data structure of states
sensor_states sstates;
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
  // Wireless Setup
  wifi.setupAP();
  wifi.setupServer();
}

// Main loop
void loop() {
  // Refresh the current hits
  // Set the current time before starting loop
  astates.current_time = millis();
  // Sensors
  sensors.probe(sstates);
  if (millis() - astates.last_update_time >= 500 || firstPoll) {
    sensors.ultrasonic_poll(sstates);
    astates.last_update_time = millis();
    if (firstPoll) {
      firstPoll = false;
      Serial.println("Sneaky first poll completed!");
    }
  }
  if (millis() - astates.last_server_time >= 2000) {
    printCurrentInfo();
    astates.last_server_time = millis();
  }
}

void printCurrentInfo() {
  String data = "L:";
  data += 100;
  data += ",R:";
  data += 100;
  data += ",D:";
  data += String(sensors.getUltrasonicDistance());
  wifi.messageClient(data);
}
