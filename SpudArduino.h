#ifndef SPUD_ARDUINO_H
#define SPUD_ARDUINO_H

#define DELAY_TIME 10
#define UPDATE_INTERVAL 100  // Set the desired update interval in milliseconds

// Data structure to store Arduino info
struct arduino_states {
  unsigned long start_time;
  unsigned long current_time;
  unsigned long last_update_time;
  unsigned long last_server_time;
};

#endif