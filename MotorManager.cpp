#include "MotorManager.h"
#include <Serial.h>

// Entry point each loop for MotorManager
// checks if the left and right motors are
// enabled or disabled.
void MotorManager::probe(motor_states &mstates) {
  if (mstates.left) {
    Serial.println("Left motor go!");
    digitalWrite(L_MOTOR_IN1, HIGH);
    digitalWrite(L_MOTOR_IN2, LOW);
  } else {
    digitalWrite(L_MOTOR_IN1, LOW);
    digitalWrite(L_MOTOR_IN2, LOW);
  }
  if (mstates.right) {
    Serial.println("Right motor go!");
    digitalWrite(R_MOTOR_IN1, HIGH);
    digitalWrite(R_MOTOR_IN2, LOW);
  } else {
    digitalWrite(R_MOTOR_IN1, LOW);
    digitalWrite(R_MOTOR_IN2, LOW);
  }
}

// Setup function for MotorManager to
// set the pins and specify OUTPUT
void MotorManager::pinSetup() {
  pinMode(L_MOTOR_IN1, OUTPUT);
  pinMode(L_MOTOR_IN2, OUTPUT);
  pinMode(R_MOTOR_IN1, OUTPUT);
  pinMode(R_MOTOR_IN2, OUTPUT);
}
