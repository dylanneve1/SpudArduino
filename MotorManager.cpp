#include "MotorManager.h"
#include <Serial.h>

// Entry point each loop for MotorManager
// checks if the left and right motors are
// enabled or disabled.
void MotorManager::probe(motor_states &mstates) {
  if (mstates.left != mstates.prev_left) {
    if (mstates.left == MSTATE_FORWARD) {
      mstates.prev_left = MSTATE_FORWARD;
      Serial.println("Left motor change:");
      Serial.println("mstates.left=MSTATE_FORWARD");
      Serial.print("mstates.left_speed=");
      Serial.println(mstates.left_speed);
      analogWrite(L_MOTOR_EN, mstates.left_speed);
      if (mstates.left_needs_update) {
        mstates.left_needs_update = false;
      }
      digitalWrite(L_MOTOR_IN1, HIGH);
      digitalWrite(L_MOTOR_IN2, LOW);
    } else if (mstates.left == MSTATE_BACKWARDS) {
      mstates.prev_left = MSTATE_FORWARD;
      Serial.println("Left motor change:");
      Serial.println("mstates.left=MSTATE_BACKWARDS");
      Serial.print("mstates.left_speed=");
      Serial.println(mstates.left_speed);
      analogWrite(L_MOTOR_EN, mstates.left_speed);
      if (mstates.left_needs_update) {
        mstates.left_needs_update = false;
      }
      digitalWrite(L_MOTOR_IN1, HIGH);
      digitalWrite(L_MOTOR_IN2, LOW);
    } else if (mstates.left == MSTATE_OFF) {
      mstates.prev_left = MSTATE_OFF;
      analogWrite(L_MOTOR_EN, 255);
      digitalWrite(L_MOTOR_IN1, LOW);
      digitalWrite(L_MOTOR_IN2, LOW);
    }
  }
  if (mstates.right != mstates.prev_right) {
    if (mstates.right == MSTATE_FORWARD) {
      mstates.prev_right = MSTATE_FORWARD;
      Serial.println("Right motor change:");
      Serial.println("mstates.right=MSTATE_FORWARD");
      //Serial.print("mstates.right_speed=");
      //Serial.println(mstates.right_speed);
      analogWrite(R_MOTOR_EN, mstates.right_speed);
      if (mstates.right_needs_update) {
        mstates.right_needs_update = false;
      }
      digitalWrite(R_MOTOR_IN1, HIGH);
      digitalWrite(R_MOTOR_IN2, LOW);
    } else if (mstates.right == MSTATE_BACKWARDS) {
      mstates.prev_right = MSTATE_FORWARD;
      Serial.println("Right motor change:");
      Serial.println("mstates.right=MSTATE_BACKWARDS");
      Serial.print("mstates.right_speed=");
      Serial.println(mstates.right_speed);
      analogWrite(R_MOTOR_EN, mstates.right_speed);
      if (mstates.right_needs_update) {
        mstates.right_needs_update = false;
      }
      digitalWrite(R_MOTOR_IN1, HIGH);
      digitalWrite(R_MOTOR_IN2, LOW);
    } else if (mstates.right == MSTATE_OFF) {
      mstates.prev_right = MSTATE_OFF;
      analogWrite(R_MOTOR_EN, 255);
      digitalWrite(R_MOTOR_IN1, LOW);
      digitalWrite(R_MOTOR_IN2, LOW);
    }
  }
  if (mstates.left_needs_update) {
    analogWrite(L_MOTOR_EN, mstates.left_speed);
    mstates.left_needs_update = false;
    //Serial.print("mstates.right_speed=");
    //Serial.println(mstates.right_speed);
  }
  if (mstates.right_needs_update) {
    analogWrite(R_MOTOR_EN, mstates.right_speed);
    mstates.right_needs_update = false;
    //Serial.print("mstates.right_speed=");
    //Serial.println(mstates.right_speed);
  }
}

// Setup function for MotorManager to
// set the pins and specify OUTPUT
void MotorManager::pinSetup() {
  pinMode(L_MOTOR_EN, OUTPUT);
  pinMode(R_MOTOR_EN, OUTPUT);
  pinMode(L_MOTOR_IN1, OUTPUT);
  pinMode(L_MOTOR_IN2, OUTPUT);
  pinMode(R_MOTOR_IN1, OUTPUT);
  pinMode(R_MOTOR_IN2, OUTPUT);
}
