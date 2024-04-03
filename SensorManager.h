#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "SpudArduino.h"

// Right and left motor macros
#define LEFT_MOTOR_ENABLE 0
#define RIGHT_MOTOR_ENABLE 1
#define LEFT_MOTOR_DISABLE 2
#define RIGHT_MOTOR_DISABLE 3
#define LEFT_MOTOR_TURN 4
#define RIGHT_MOTOR_TURN 5

// IR sensor events
#define LEVENT 11
#define REVENT 12

// IR sensor states
#define SENSOR_LOW 13
#define SENSOR_HIGH 14

// IR sensor pins
#define LEYE 4
#define REYE 13

// Ultrasonic pins
#define US_TRIG 8
#define US_ECHO 9

// How often to poll ultrasonic
#define US_POLL_TIMEFRAME 1000

// Left and right motor encoder
#define R_MOTOR_ENC 2
#define L_MOTOR_ENC 3

// Left and right motor pins
#define R_MOTOR_IN1 6
#define R_MOTOR_IN2 5
#define L_MOTOR_IN1 11
#define L_MOTOR_IN2 12

#define REVOLUTION_DISTANCE 0.204

// Enable pins for HBridge motors
// can control voltage delivered
// and as such the speed
#define L_MOTOR_EN 10
#define R_MOTOR_EN 7

// Data structure to store IR sensor
// states between loops, we will default
// to LOW state
struct sensor_states {
  int ir_left = SENSOR_LOW;
  int ir_right = SENSOR_LOW;
  int left_motor_speed = MOTOR_SPEED_MAX;
  int right_motor_speed = MOTOR_SPEED_MAX;
  bool firstPoll = true;
  int pidCoef;
  bool pidEnabled = false;
  int usdist;
  int reference_speed;
  int converted_reference_speed =80;
  double error;
  bool first_us_ret = false;
  int last_us_ret;
  int bad_ret_cnt = 0;
};

// SensorManager class
class SensorManager {

public:
  void probe(int work, sensor_states &sstates, arduino_states &astates);
  void pinSetup();
  int getUltrasonicDistance(sensor_states &sstates);
  void changeMotor(int motor, sensor_states &sstates, arduino_states &astates);
  double checkWheelEnc(volatile int leftRevolutions, volatile int rightRevolutions);
  void calculateBuggySpeed(sensor_states &sstates, arduino_states &astates);

private:
  void alignBuggySpeed(sensor_states &sstates, arduino_states &astates);
  double computePID(double inp, arduino_states &astates, sensor_states &sstates);
  void ir_sensor_poll(sensor_states &sstates, arduino_states &astates);
  void ultrasonic_poll(int work, sensor_states &sstates, arduino_states &astates);
  double kp = (1 / 7.3);
  double ki = 1 / 20;
  double kd = 2;
  double elapsedTime;
  double lastError;
  const double setPoint = 20;
  double cumError, rateError;
};

#endif
