#include <WiFiS3.h>
#include <iostream>
#include <sstream>
#include <string>
#include <regex>

// Function prototypes
void setup();
void loop();
void LencoderISR();
void RencoderISR();
void ir_sensor_poll();
void ir_sensor_event(int event, int intensity);
void ultrasonic_poll();
int getUltrasonicDistance();
void changeMotor(int motor);
double checkWheelEnc();
double checkAvgSpeed();
void printCurrentInfo();
int startStopCommandReceived();

// Timeframes
#define SERVER_POLL_TIMEFRAME 2000
#define US_POLL_TIMEFRAME 500
#define SPEED_CALC_POLL_TIMEFRAME 1000

// Buggy working or idle
#define BUGGY_WORK 1
#define BUGGY_IDLE 0

// Max and min motor pwm values
// Range is currently 40
#define MOTOR_SPEED_MAX 120
#define MOTOR_SPEED_BASE 80
#define MOTOR_SPEED_NONE 0

// Sensor constants
#define SENSOR_LOW 0
#define SENSOR_HIGH 1
#define LEVENT 11
#define REVENT 12

// Motor commands
#define LEFT_MOTOR_ENABLE_CMD 0
#define RIGHT_MOTOR_ENABLE_CMD 1
#define LEFT_MOTOR_DISABLE_CMD 2
#define RIGHT_MOTOR_DISABLE_CMD 3

// Encoder Pins
#define L_MOTOR_ENC 3
#define R_MOTOR_ENC 2

// IR Sensor Pins
#define LEYE 4
#define REYE 13

// Ultrasonic Pins
#define US_TRIG 8
#define US_ECHO 9

// Motor Control Pins
#define R_MOTOR_IN1 6
#define R_MOTOR_IN2 5
#define L_MOTOR_IN1 11
#define L_MOTOR_IN2 12
#define L_MOTOR_EN 10
#define R_MOTOR_EN 7

// Encoder wheel revolution distance
#define REVOLUTION_DISTANCE 0.204

// Data structures
struct sensor_states {
  int ir_left = SENSOR_LOW;
  int ir_right = SENSOR_LOW;
  int left_motor_speed = MOTOR_SPEED_NONE;
  int right_motor_speed = MOTOR_SPEED_NONE;
  volatile int leftRevolutions = 0;
  volatile int rightRevolutions = 0;
};

struct arduino_states {
  bool fp = true;
  int work = BUGGY_IDLE;
  int reference_speed = -1;
  unsigned long start_time = 0;
  unsigned long current_time = 0;
  unsigned long last_update_time = 0;
  unsigned long last_server_time = 0;
  unsigned long last_distance_time = 0;
  unsigned long last_speed_calc_time = 0;
  bool first_distance_checked = false;
  double dist = 0;
  double last_dist = 0;
  double avg_v = 0;
};
