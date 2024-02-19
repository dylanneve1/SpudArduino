#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

// WiFIManager class
class WiFiManager {
  public:
    void setupAP();
    void setupServer();
    void messageClient(String message);
};

#endif
