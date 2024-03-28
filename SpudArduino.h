#ifndef SPUD_ARDUINO_H
#define SPUD_ARDUINO_H

// Enable and disable WiFi
#define WIFI_ENABLED 0

// Server poll frequency
#define SERVER_POLL_TIMEFRAME 2000
#define SPEED_CALC_POLL_TIMEFRAME 1000

// Buggy work states
#define BUGGY_WORK 1
#define BUGGY_IDLE 0

#define MOTOR_SPEED_MAX 150
#define MOTOR_SPEED_MIN 0
#define MOTOR_SPEED_TURN 70
#define MOTOR_SPEED_PID 100

#define PID_MULTIPLE 12

void encoderISR();

// Data structure to store Arduino info
struct arduino_states {
  unsigned long start_time;
  unsigned long current_time;
  unsigned long last_update_time;
  unsigned long last_server_time;
  unsigned long last_distance_time;
  unsigned long last_speed_calc_time;
  unsigned long last_pid_calc_time;
  bool first_distance_checked = false;
  double dist, last_dist, avg_v;
};

#endif
