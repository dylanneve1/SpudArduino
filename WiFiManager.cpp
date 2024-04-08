// WiFiManager.cpp

#include "SpudArduino.h"
#include "WiFiManager.h"
#include <iostream>

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
    sstates.reference_speed = 15;
    return 1;
    Serial.println("WiFiManager: WiFi disabled!");
  }
  if (client.available()) {
    String command_e = client.readStringUntil('\n');
    Serial.println(command_e);
    char ch;
    int l = command_e.length();
    for (int i = 0; i < l; i++) {
      ch = command_e[i];
      //Serial.println(ch);
      if (ch == 'B') {
        work = command_e[i + 2] - '0';
        Serial.print("work: ");
        Serial.print(command_e[i + 2]);
        Serial.print(" ");
        Serial.println(work);
      } else if (ch == 'S') {
        std::string eh = command_e.c_str();
        std::string speed = eh.substr(i + 2);
        sstates.reference_speed = (int)stoi(speed);
      }
    }
    Serial.print("work: ");
    Serial.println(work);
    Serial.print("speed (ref): ");
    Serial.println(sstates.reference_speed);
    if (work == BUGGY_WORK) {
      Serial.println("It should start");
      sstates.converted_reference_speed = INITIAL_REF_SPEED;
      lastOne = true;
      return 1;
    } else if (work == BUGGY_IDLE) {
      Serial.println("It should stop.");
      sstates.converted_reference_speed = INITIAL_REF_SPEED;
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
  Serial.print("reference_speed: ");
  Serial.println(sstates.reference_speed);
  Serial.print("avg_v: ");
  Serial.println(astates.avg_v);

  String data = "L:";
  data += sstates.left_motor_speed;
  data += ",R:";
  data += sstates.right_motor_speed;
  data += ",D:";
  data += String(sstates.usdist);
  data += ",T:";
  data += astates.dist;
  data += ",V:";
  data += String(std::to_string((int)astates.avg_v).c_str());
  data += ",E:";
  data += String(sstates.error);
  Serial.println(data);
  messageClient(data);
}
