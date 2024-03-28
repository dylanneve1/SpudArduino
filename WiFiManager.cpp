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
  const char *message_formatted = message.c_str();
  if (client.connected()) {
    client.write(message_formatted);
    client.write("\n");
  }
}

// Function to check for start stop command
int WiFiManager::startStopCommandReceived(sensor_states &sstates) {
  int work;
  if (WIFI_ENABLED == 0) {
    return 1; 
  }
  if (client.available()) {
    String command_e = client.readStringUntil('\n');
    std::string command = command_e.c_str();
    std::regex pattern("(B:(\d+),S:(\d+))");
    std::smatch match;
    if (std::regex_match(command, match, pattern)) {
      work = (int)std::stoi(match[1]);
      sstates.reference_speed = (int)std::stoi(match[2]);
    }
    if (work == BUGGY_WORK) {
      Serial.println("It should start");
      lastOne = true;
      return 1;
    } else if (work == BUGGY_IDLE) {
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
