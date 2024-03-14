#include "SensorManager.h"
#include <Serial.h>
#include "WiFiManager.h"
#include "SpudArduino.h"


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
  pinMode(L_MOTOR_ENC, INPUT_PULLUP);
  pinMode(R_MOTOR_ENC, INPUT_PULLUP);
}

// Entry point for SensorManager class
// polls each sensor and performs any
// necessary updates to the data structs
void SensorManager::probe(int work, sensor_states &sstates) {
  if (work == BUGGY_WORK) {
    ir_sensor_poll(sstates);
  }
}

// void SensorManager::controlmode(char mode, sensor_states &sstates) {
//   if (mode == 'U') {
//     sstates.control_mode = MANUAL_USER_CONTROL;
//   } else if (mode == 'R') {
//     sstates.control_mode = REFERENCE_OBJECT_CONTROL;
//   }
// }

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
        changeMotor(LEFT_MOTOR_ENABLE, sstates);
        Serial.println("Left motor enabled!");
      } else {
        changeMotor(LEFT_MOTOR_DISABLE, sstates);
        Serial.println("Left motor disabled!");
      }
    }
  } else if (event == REVENT) {
    if (intensity != sstates.ir_right) {
      Serial.println("sensor_event: right state changed!");
      sstates.ir_right = intensity;
      if (intensity == SENSOR_HIGH) {
        changeMotor(RIGHT_MOTOR_ENABLE, sstates);
        Serial.println("Right motor enabled!");
      } else {
        changeMotor(RIGHT_MOTOR_DISABLE, sstates);
        Serial.println("Right motor disabled!");
      }
    }
  }
}

// Function to change motor states
// between on and off for the left
// and right motors
void SensorManager::changeMotor(int motor, sensor_states &sstates) {
  if (motor == LEFT_MOTOR_ENABLE) {
    analogWrite(L_MOTOR_EN, sstates.left_motor_speed);
    digitalWrite(L_MOTOR_IN1, HIGH);
    digitalWrite(L_MOTOR_IN2, LOW);
  } else if (motor == RIGHT_MOTOR_ENABLE) {
    analogWrite(R_MOTOR_EN, sstates.right_motor_speed);
    digitalWrite(R_MOTOR_IN1, HIGH);
    digitalWrite(R_MOTOR_IN2, LOW);
  } else if (motor == LEFT_MOTOR_DISABLE) {
    analogWrite(L_MOTOR_EN, MOTOR_SPEED_MIN);
    digitalWrite(L_MOTOR_IN1, LOW);
    digitalWrite(L_MOTOR_IN2, LOW);
    sstates.left_motor_speed = MOTOR_SPEED_MIN;
  } else if (motor == RIGHT_MOTOR_DISABLE) {
    analogWrite(R_MOTOR_EN, MOTOR_SPEED_MIN);
    digitalWrite(R_MOTOR_IN1, LOW);
    digitalWrite(R_MOTOR_IN2, LOW);
    sstates.right_motor_speed = MOTOR_SPEED_MIN;
  }
}

void SensorManager::ultrasonic_poll(int work, sensor_states &sstates) {
  // Probe devices passing them states
  // incase changes have occurred
  // Check distance with ultrasonic sensor
  if (work == BUGGY_WORK) {
    int distance = getUltrasonicDistance();

    if (distance < 20.0) {
      Serial.print("Distance detected: ");
      Serial.print(distance);
      Serial.println(" cm");
      Serial.println("YOU NEED TO STOP!");
      changeMotor(LEFT_MOTOR_DISABLE, sstates);
      changeMotor(RIGHT_MOTOR_DISABLE, sstates);
      return;
    }
    if (sstates.control_mode == REFERENCE_OBJECT_CONTROL) {
      if (distance > 10 && distance < 40) {
        sstates.left_motor_speed = 200 + distance / 2;
        sstates.right_motor_speed = 200 + distance / 2;
      } else {
        sstates.left_motor_speed = 240;
        sstates.right_motor_speed = 240;
      }
      if (sstates.ir_left == SENSOR_HIGH) {
        changeMotor(LEFT_MOTOR_ENABLE, sstates);
      }
      if (sstates.ir_right == SENSOR_HIGH) {
        changeMotor(RIGHT_MOTOR_ENABLE, sstates);
      }
      Serial.println(sstates.left_motor_speed);
    }
    // else if (sstates.control_mode== MANUAL_USER_CONTROL)
    // {
      
    // }
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

double SensorManager::checkWheelEnc(volatile int leftRevolutions, volatile int rightRevolutions) {
  double ret = 0;
  ret = (REVOLUTION_DISTANCE / 2) * leftRevolutions;
  ret += (REVOLUTION_DISTANCE / 2) * rightRevolutions;
  ret = ret * 0.25;
  return ret;
}

double SensorManager::checkAvgSpeed(double &dist, double &last_dist, unsigned int &last_time) {
  double ret = 0;
  delta_dist = dist - last_dist;
  delta_time = millis() - last_time;
  ret = delta_dist / delta_time;
  last_time = millis();
  last_dist = dist;
  return ret;
}
