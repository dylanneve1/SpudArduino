// WiFiManager.h

#ifndef WiFiManager_h
#define WiFiManager_h

#include "Arduino.h"
#include <WiFiS3.h>
#include "SensorManager.h"
#include <iostream>
#include <sstream>
#include <string>
#include <regex>

class WiFiManager {
  public:
    WiFiManager();  // Constructor
    void setupWiFi();
    void probe(arduino_states &astates, sensor_states &sstates);
    void messageClient(String message);
    int startStopCommandReceived(sensor_states &sstates);
  private:
    void printCurrentInfo(arduino_states &astates, sensor_states &sstates);
    WiFiServer server;
    WiFiClient client;
    bool startStopCommand;
    bool lastOne = false;
};

#endif
