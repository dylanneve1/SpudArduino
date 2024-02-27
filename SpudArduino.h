#ifndef SPUD_ARDUINO_H
#define SPUD_ARDUINO_H

// Server poll frequency
#define SERVER_POLL_TIMEFRAME 2000

// Buggy work states
#define BUGGY_WORK 1
#define BUGGY_IDLE 0

void encoderISR();

// Data structure to store Arduino info
struct arduino_states {
  unsigned long start_time;
  unsigned long current_time;
  unsigned long last_update_time;
  unsigned long last_server_time;
};

#endif