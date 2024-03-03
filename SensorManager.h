#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

// Right and left motor macros
#define LEFT_MOTOR_ENABLE   0
#define RIGHT_MOTOR_ENABLE  1
#define LEFT_MOTOR_DISABLE  2
#define RIGHT_MOTOR_DISABLE 3

// Left and right motor encoder
// Left and right motor pins
#define R_MOTOR_IN1 6
#define R_MOTOR_IN2 5
#define L_MOTOR_IN1 11
#define L_MOTOR_IN2 12

// Enable pins for HBridge motors
// can control voltage delivered
// and as such the speed
#define L_MOTOR_EN 10
#define R_MOTOR_EN 7

// Motor Speeds
#define MOTOR_SPEED_MAX 255
#define MOTOR_SPEED_MIN 0

// Data structure to store IR sensor
// states between loops, we will default
// to LOW state
struct sensor_states {
  int left_motor_speed;
  int right_motor_speed;
};

// SensorManager class
class SensorManager {
private:
  sensor_states states;
public:
  SensorManager();
  void pinSetup();
  void changeMotor(int motor);
};

#endif
