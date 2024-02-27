// WiFiManager.cpp

#include "SpudArduino.h"
#include "WiFiManager.h"

// Constructor for WiFiManager class
WiFiManager::WiFiManager()
  : server(5200), client(), startStopCommand(false) {
}

// Function to setup Arduino access point
void WiFiManager::setupAP() {
  char ssid[] = "SpudArduino";
  char pass[] = "Arduino2024";

  WiFi.beginAP(ssid, pass);

  IPAddress ip = WiFi.localIP();

  Serial.print("IP Address: ");
  Serial.println(ip);
}

// Setup the server connection
void WiFiManager::setupServer() {
  server.begin();
}

// Check that client is avaliable
// and format message to using c_str()
void WiFiManager::messageClient(String message) {
  client = server.available();
  const char* message_formatted = message.c_str();
  if (client.connected()) {
    client.write(message_formatted);
    client.write("\n");
  }
}

// Function to check for start stop command
int WiFiManager::startStopCommandReceived() {
  if (client.available()) {
    String command = client.readStringUntil('\n');

    if (command == "1") {
      Serial.println("It should start");
      lastOne = true;
      return 1;
    } else if (command == "0") {
      Serial.println("It should stop.");
      lastOne = false;
      return 0;
    }
  }
  // If no command is recieved
  // stay the same as last
  if (lastOne) {
    return 1;
  } else {
    return 0;
  }
}
