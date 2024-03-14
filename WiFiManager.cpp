// WiFiManager.cpp

#include "SpudArduino.h"
#include "WiFiManager.h"
#include <string>
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
// String [] values= command.split(",");
// if (values.length>=2) {
// String[] statestring= values[0].split(":");
// int state= int(statestring[1]);
// String[] motorstring= values[1].split(":");
// int motorspeed= stoi(motorstring[1].trim())}
    if (command == "1") {
      Serial.println("It should start");
      lastOne = true;
      return 1;
    } else if (command== "0") {
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


// // Function to check for start stop command
// int WiFiManager::startStopCommandReceived() {
//   if (client.available()) {
//     String command = client.readStringUntil('\n');
//     if (command.startsWith("B:") && command.indexOf("M:") != -1 && command.indexOf("S:") != -1) {
//       int startStopIndex = command.indexOf("B:") + 2;
//       int modeIndex = command.indexOf("M:");
//       int speedIndex = command.indexOf("S:");

//       int startStop = command.substring(startStopIndex, modeIndex).toInt();
//       char mode = command.charAt(modeIndex + 2);
//       float speed = command.substring(speedIndex + 2).toFloat();

//       // Handle start/stop command
//       if (startStop == 1) {
//         Serial.println("It should start");
//         lastOne = true;
//         // Send appropriate commands based on mode and speed
//         if (mode == 'U') {
//           // Implement logic for manual user mode
//           Serial.println("Switching to manual.");
          
//         } else if (mode == 'R') {
//           // Implement logic for reference object control mode
//           Serial.println("Switching to reference mode.")
//         }
//         return 1;
//       } else if (startStop == 0) {
//         Serial.println("It should stop.");
//         lastOne = false;
//         // Send stop command
//         // Implement logic to stop motors, if necessary
//         return 0;
//       }
//     }
//   }
//   // If no command is received, stay the same as last
//   if (lastOne) {
//     return 1;
//   } else {
//     return 0;
//   }
// }


int WiFiManager::motorspeed(int motors)
{
  if (motors==-2)
  return -2;
  else 
  return 1; 
}
