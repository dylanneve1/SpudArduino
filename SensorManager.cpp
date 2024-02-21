#include "SensorManager.h"
#include <Serial.h>
#include "GuidanceManager.h"
#include "MotorManager.h"

// Setup pins for the SensorManager
void SensorManager::pinSetup() {
  pinMode(LEYE, INPUT);
  pinMode(REYE, INPUT);
  pinMode(US_TRIG, OUTPUT);
  pinMode(US_ECHO, INPUT);
}

// Entry point for SensorManager class
// polls each sensor and performs any
// necessary updates to the data structs
void SensorManager::probe(sensor_states &sstates, motor_states &mstates, GuidanceManager &guidance) {
  ir_sensor_poll(sstates, mstates, guidance);
}

void SensorManager::ir_sensor_poll(sensor_states &sstates, motor_states &mstates, GuidanceManager &guidance) {
  if (digitalRead(LEYE) != HIGH) {
    ir_sensor_event(LEVENT, SENSOR_LOW, sstates, mstates, guidance);
  } else {
    ir_sensor_event(LEVENT, SENSOR_HIGH, sstates, mstates, guidance);
  }
  if (digitalRead(REYE) != HIGH) {
    ir_sensor_event(REVENT, SENSOR_LOW, sstates, mstates, guidance);
  } else {
    ir_sensor_event(REVENT, SENSOR_HIGH, sstates, mstates, guidance);
  }
}

// IR Sensor Event
void SensorManager::ir_sensor_event(int event, int intensity, sensor_states &sstates, motor_states &mstates, GuidanceManager &guidance) {
  // Check if Left or Right IR Sensor
  // If intensity is different from
  // the current sensor states then
  // we should log the state change
  // as well as update the stored
  if (event == LEVENT) {
    if (intensity != sstates.ir_left) {
      Serial.println("sensor_event: left state changed!");
      sstates.ir_left = intensity;
      if (intensity == SENSOR_HIGH) {
        mstates.left = MSTATE_FORWARD;
        Serial.println("Left motor enabled!");
      } else {
        mstates.left = MSTATE_OFF;
        Serial.println("Left motor disabled!");
        guidance.createEntry();
      }
    }
  } else if (event == REVENT) {
    if (intensity != sstates.ir_right) {
      Serial.println("sensor_event: right state changed!");
      sstates.ir_right = intensity;
      if (intensity == SENSOR_HIGH) {
        mstates.right = MSTATE_FORWARD;
        Serial.println("Right motor enabled!");
      } else {
        mstates.right = MSTATE_OFF;
        Serial.println("Right motor disabled!");
        guidance.createEntry();
      }
    }
  }
}

void SensorManager::ultrasonic_poll(motor_states &mstates, sensor_states &sstates) {
  // Probe devices passing them states
  // incase changes have occurred
   // Check distance with ultrasonic sensor
  int distance = getUltrasonicDistance();

  if (distance < 20.0) {
    Serial.print("Distance detected: ");
    Serial.print(distance);
    Serial.println(" cm");
    Serial.println("YOU NEED TO STOP!");
    mstates.left = MSTATE_OFF;
    mstates.right = MSTATE_OFF;
    return;
  }
  if (sstates.ir_left == SENSOR_HIGH && mstates.left == MSTATE_OFF) {
    mstates.left = MSTATE_FORWARD;
  }
  if (sstates.ir_right == SENSOR_HIGH && mstates.right == MSTATE_OFF) {
    mstates.right = MSTATE_FORWARD;
  }
}

// Function to get ultrasonic distance
int SensorManager::getUltrasonicDistance() {
  digitalWrite(US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_TRIG, LOW);
  long duration = pulseIn(US_ECHO, HIGH);
  return duration / 58;
}
