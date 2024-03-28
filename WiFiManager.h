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
    void messageClient(String message);
    int startStopCommandReceived();
  private:
    WiFiServer server;
    WiFiClient client;
    bool startStopCommand;
    bool lastOne = false;
};

#endif
