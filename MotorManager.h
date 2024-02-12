#ifndef MOTOR_MANAGER_H
#define MOTOR_MANAGER_H

// Pins for controlling motor
// IN1 = HIGH && IN2 = LOW results in forwards
// IN1 = LOW && IN2 = HIGH results in backwards
#define R_MOTOR_IN1    6
#define R_MOTOR_IN2    5
#define L_MOTOR_IN1   11
#define L_MOTOR_IN2   12

// Enable pins for HBridge motors
// can control voltage delivered
// and as such the speed
#define L_MOTOR_EN    10
#define R_MOTOR_EN     7

// Different states for motors
// forwards, backwards or off
#define MSTATE_FORWARD    90
#define MSTATE_BACKWARDS  91
#define MSTATE_OFF        92

// Data structure to store left and right
// motor states, previous state and
// speed of each motor
struct motor_states {
  int left = MSTATE_OFF;
  int right = MSTATE_OFF;
  int prev_left = MSTATE_OFF;
  int prev_right = MSTATE_OFF;
  int left_speed = 220;
  int left_needs_update = false;
  int right_speed = 220;
  int right_needs_update = false;
};

// MotorManager class
class MotorManager {
  public:
    void probe(motor_states &mstates);
    void pinSetup();
};

#endif
