#ifndef MOTOR_MANAGER_H
#define MOTOR_MANAGER_H

// Pins for controlling motor
// IN1 = HIGH && IN2 = LOW results in forwards
// IN1 = LOW && IN2 = HIGH results in backwards
#define R_MOTOR_IN1    6
#define R_MOTOR_IN2    5
#define L_MOTOR_IN1   11
#define L_MOTOR_IN2   12

// Data structure to store left and right
// motor states
struct motor_states {
  bool left = false;
  bool right = false;
};

// MotorManager class
class MotorManager {
  public:
    void probe(motor_states &mstates);
    void pinSetup();
};

#endif
