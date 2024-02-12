#include "SensorManager.h"
#include <Serial.h>

// Setup pins for the SensorManager
void SensorManager::pinSetup() {
  pinMode(LEYE, INPUT);
  pinMode(REYE, INPUT);
}

// Entry point for SensorManager class
// polls each sensor and performs any
// necessary updates to the data structs
void SensorManager::probe(sensor_states &sstates, motor_states &mstates) {
  ir_sensor_poll(sstates, mstates);
}

int SensorManager::ir_sensor_poll(sensor_states &sstates, motor_states &mstates) {
  if (digitalRead(LEYE) != HIGH) {
    ir_sensor_event(LEVENT, SENSOR_LOW, sstates, mstates);
  } else {
    ir_sensor_event(LEVENT, SENSOR_HIGH, sstates, mstates);
  }
  if (digitalRead(REYE) != HIGH) {
    ir_sensor_event(REVENT, SENSOR_LOW, sstates, mstates);
  } else {
    ir_sensor_event(REVENT, SENSOR_HIGH, sstates, mstates);
  }
}

// IR Sensor Event
int SensorManager::ir_sensor_event(int event, int intensity, sensor_states &sstates, motor_states &mstates) {
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
      }
    }
  }
}
