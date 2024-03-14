// WiFiManager.h

#ifndef WiFiManager_h
#define WiFiManager_h

#include "Arduino.h"
#include <WiFiS3.h>
#include "SensorManager.h"

class WiFiManager {
public:
  WiFiManager();  // Constructor
  void setupAP();
  void setupServer();
  void messageClient(String message);
  int startStopCommandReceived();
  int motorspeed(int motors);
private:
  WiFiServer server;
  WiFiClient client;
  bool startStopCommand;
  bool lastOne = false;
};

#endif
