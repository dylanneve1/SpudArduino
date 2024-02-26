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

// Flag to control the motors
bool motorsEnabled = false;

// Check if US has been polled
bool firstPoll = true;

bool R_ENC = true;

int dist = 0;

int work = 0;

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

  if (digitalRead(L_MOTOR_ENC) == HIGH && R_ENC) {
    dist++;
    R_ENC = false;
  }
  if (digitalRead(L_MOTOR_ENC) == LOW && !R_ENC) {
    R_ENC = true;
  }

  work = wifi.startStopCommandReceived();
  if (work != 1) {
    sensors.changeMotor(2);
    sensors.changeMotor(3);
  }
  
  // Set the current time before starting loop
  astates.current_time = millis();
  // Sensors
  sensors.probe(work, sstates);
  if (millis() - astates.last_update_time >= 500 || firstPoll) {
    sensors.ultrasonic_poll(work, sstates);
    astates.last_update_time = millis();
    Serial.println(dist);
  }
  if (firstPoll) {
    firstPoll = false;
    Serial.println("Sneaky first poll completed!");
  }
  if (millis() - astates.last_server_time >= 2000) {
    // Print current information
    printCurrentInfo();
    // Check for start/stop command from Processing
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
