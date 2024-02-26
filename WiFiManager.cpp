// WiFiManager.cpp

#include "SpudArduino.h"
#include "WiFiManager.h"


WiFiManager::WiFiManager()
  : server(5200), client(), startStopCommand(false) {
}

void WiFiManager::setupAP() {
  char ssid[] = "SpudArduino";
  char pass[] = "Arduino2024";

  WiFi.beginAP(ssid, pass);

  IPAddress ip = WiFi.localIP();

  Serial.print("IP Address: ");
  Serial.println(ip);
}

void WiFiManager::setupServer() {
  server.begin();
}

void WiFiManager::messageClient(String message) {
  client = server.available();
  const char* message_formatted = message.c_str();
  if (client.connected()) {
    client.write(message_formatted);
    client.write("\n");
  }
}

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
  if (lastOne) {
    return 1;
  } else {
    return 0;
  }
}
