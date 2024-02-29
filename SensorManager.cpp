#include "SensorManager.h"
#include <Serial.h>
#include "SpudArduino.h"


SensorManager::SensorManager(sensor_states* states) {
  this->states = states;
  this->states->left_motor_speed = 0;
  this->states->right_motor_speed = 0;
}

// Setup pins for the SensorManager
void SensorManager::pinSetup() {
  pinMode(L_MOTOR_EN, OUTPUT);
  pinMode(R_MOTOR_EN, OUTPUT);
  pinMode(L_MOTOR_IN1, OUTPUT);
  pinMode(L_MOTOR_IN2, OUTPUT);
  pinMode(R_MOTOR_IN1, OUTPUT);
  pinMode(R_MOTOR_IN2, OUTPUT);
}

// Function to change motor states
// between on and off for the left
// and right motors
void SensorManager::changeMotor(int motor) {
  if (motor == LEFT_MOTOR_ENABLE) {
    analogWrite(L_MOTOR_EN, MOTOR_SPEED_MAX);
    digitalWrite(L_MOTOR_IN1, HIGH);
    digitalWrite(L_MOTOR_IN2, LOW);
    states->left_motor_speed = MOTOR_SPEED_MAX;
  } else if (motor == RIGHT_MOTOR_ENABLE) {
    analogWrite(R_MOTOR_EN, MOTOR_SPEED_MAX);
    digitalWrite(R_MOTOR_IN1, HIGH);
    digitalWrite(R_MOTOR_IN2, LOW);
    states->right_motor_speed = MOTOR_SPEED_MAX;
  } else if (motor == LEFT_MOTOR_DISABLE) {
    analogWrite(L_MOTOR_EN, MOTOR_SPEED_MIN);
    digitalWrite(L_MOTOR_IN1, LOW);
    digitalWrite(L_MOTOR_IN2, LOW);
    states->left_motor_speed = MOTOR_SPEED_MIN;
  } else if (motor == RIGHT_MOTOR_DISABLE) {
    analogWrite(R_MOTOR_EN, MOTOR_SPEED_MIN);
    digitalWrite(R_MOTOR_IN1, LOW);
    digitalWrite(R_MOTOR_IN2, LOW);
    states->right_motor_speed = MOTOR_SPEED_MIN;
  }
}
