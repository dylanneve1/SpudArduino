#include <WiFiS3.h>
#include "WiFiManager.h"

WiFiServer server(5200);

void WiFiManager::setupAP() {
  char ssid[] = "SpudArduino";
  char pass[] = "Arduino2024";
  
  WiFi.beginAP(ssid, pass);

  IPAddress ip = WiFi.localIP();

  Serial.print("IP Address: ");
  Serial.println(ip);
  //delay(10000);
}

void WiFiManager::setupServer() {
  server.begin();
}

void WiFiManager::messageClient(String message) {
  WiFiClient client = server.available();
  const char* message_formatted = message.c_str();
  if (client.connected()) { 
    client.write(message_formatted);
    client.write("\n");
  } 
}
