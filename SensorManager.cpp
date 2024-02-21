#include "SensorManager.h"
#include <Serial.h>

// Setup pins for the SensorManager
void SensorManager::pinSetup() {
  pinMode(LEYE, INPUT);
  pinMode(REYE, INPUT);
  pinMode(US_TRIG, OUTPUT);
  pinMode(US_ECHO, INPUT);
  pinMode(L_MOTOR_EN, OUTPUT);
  pinMode(R_MOTOR_EN, OUTPUT);
  pinMode(L_MOTOR_IN1, OUTPUT);
  pinMode(L_MOTOR_IN2, OUTPUT);
  pinMode(R_MOTOR_IN1, OUTPUT);
  pinMode(R_MOTOR_IN2, OUTPUT);
}

// Entry point for SensorManager class
// polls each sensor and performs any
// necessary updates to the data structs
void SensorManager::probe(sensor_states &sstates) {
  ir_sensor_poll(sstates);
}

void SensorManager::ir_sensor_poll(sensor_states &sstates) {
  if (digitalRead(LEYE) != HIGH) {
    ir_sensor_event(LEVENT, SENSOR_LOW, sstates);
  } else {
    ir_sensor_event(LEVENT, SENSOR_HIGH, sstates);
  }
  if (digitalRead(REYE) != HIGH) {
    ir_sensor_event(REVENT, SENSOR_LOW, sstates);
  } else {
    ir_sensor_event(REVENT, SENSOR_HIGH, sstates);
  }
}

// IR Sensor Event
void SensorManager::ir_sensor_event(int event, int intensity, sensor_states &sstates) {
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
        changeMotor(0);
        Serial.println("Left motor enabled!");
      } else {
        changeMotor(2);
        Serial.println("Left motor disabled!");
      }
    }
  } else if (event == REVENT) {
    if (intensity != sstates.ir_right) {
      Serial.println("sensor_event: right state changed!");
      sstates.ir_right = intensity;
      if (intensity == SENSOR_HIGH) {
        changeMotor(1);
        Serial.println("Right motor enabled!");
      } else {
        changeMotor(3);
        Serial.println("Right motor disabled!");
      }
    }
  }
}

void SensorManager::changeMotor(int motor) {
  if (motor == 0) {
    analogWrite(L_MOTOR_EN, 210);
    digitalWrite(L_MOTOR_IN1, HIGH);
    digitalWrite(L_MOTOR_IN2, LOW);
  } else if (motor == 1) {
    analogWrite(R_MOTOR_EN, 210);
    digitalWrite(R_MOTOR_IN1, HIGH);
    digitalWrite(R_MOTOR_IN2, LOW);
  } else if (motor == 2) {
    analogWrite(L_MOTOR_EN, 0);
    digitalWrite(L_MOTOR_IN1, LOW);
    digitalWrite(L_MOTOR_IN2, LOW);
  } else if (motor == 3) {
    analogWrite(R_MOTOR_EN, 0);
    digitalWrite(R_MOTOR_IN1, LOW);
    digitalWrite(R_MOTOR_IN2, LOW);
  }
}

void SensorManager::ultrasonic_poll(sensor_states &sstates) {
  // Probe devices passing them states
  // incase changes have occurred
   // Check distance with ultrasonic sensor
  int distance = getUltrasonicDistance();

  if (distance < 20.0) {
    Serial.print("Distance detected: ");
    Serial.print(distance);
    Serial.println(" cm");
    Serial.println("YOU NEED TO STOP!");
    changeMotor(2);
    return;
  }
  if (sstates.ir_left == SENSOR_HIGH) {
    changeMotor(0);
  }
  if (sstates.ir_right == SENSOR_HIGH) {
    changeMotor(1);
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
