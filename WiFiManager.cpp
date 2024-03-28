// WiFiManager.cpp

#include "SpudArduino.h"
#include "WiFiManager.h"

// Constructor for WiFiManager class
WiFiManager::WiFiManager()
  : server(5200), client(), startStopCommand(false) {
}

// Function to setup Arduino access point
void WiFiManager::setupWiFi() {
  char ssid[] = "SpudArduino";
  char pass[] = "Arduino2024";

  WiFi.beginAP(ssid, pass);

  IPAddress ip = WiFi.localIP();

  Serial.print("IP Address: ");
  Serial.println(ip);

  server.begin();
}

void WiFiManager::probe(arduino_states &astates, sensor_states &sstates) {
  if (millis() - astates.last_server_time >= SERVER_POLL_TIMEFRAME) {
    // Print current information
    printCurrentInfo(astates, sstates);
    // Check for start/stop command from Processing
    astates.last_server_time = millis();
  }
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
  return 1;
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

void WiFiManager::printCurrentInfo(arduino_states &astates, sensor_states &sstates) {
  String data = "L:";
  data += sstates.left_motor_speed;
  data += ",R:";
  data += sstates.right_motor_speed;
  data += ",D:";
  data += String(sstates.usdist);
  data += ",T:";
  data += astates.dist;
  messageClient(data);
}
