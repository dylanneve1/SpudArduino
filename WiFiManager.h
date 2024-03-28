// WiFiManager.h

#ifndef WiFiManager_h
#define WiFiManager_h

#include "Arduino.h"
#include <WiFiS3.h>
#include "SensorManager.h"

class WiFiManager {
  public:
    WiFiManager();  // Constructor
    void setupWiFi();
    void probe(arduino_states &astates, sensor_states &sstates);
    void messageClient(String message);
    int startStopCommandReceived();
  private:
    void printCurrentInfo(arduino_states &astates, sensor_states &sstates);
    WiFiServer server;
    WiFiClient client;
    bool startStopCommand;
    bool lastOne = false;
};

#endif
