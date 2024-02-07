const int LEYE = 4; 
const int REYE = 13;

const int R_MOTOR_IN1 = 6;
const int R_MOTOR_IN2 = 5;
const int L_MOTOR_IN1 = 11;
const int L_MOTOR_IN2 = 12;

#include "sensor.h"

struct sensor_states {
  bool motor_left = false;
  bool motor_right = false;
  int ir_left = SENSOR_LOW;
  int ir_right = SENSOR_LOW;
};

sensor_states sstates;

void setup() {  // Begin serial
  Serial.begin(9600);
  // Sensor pins
  pinMode(LEYE, INPUT);
  pinMode(REYE, INPUT);
  pinMode(L_MOTOR_IN1, OUTPUT);
  pinMode(L_MOTOR_IN2, OUTPUT);
  pinMode(R_MOTOR_IN1, OUTPUT);
  pinMode(R_MOTOR_IN2, OUTPUT);
}

// IR Sensor Event
void ir_sensor_event(int event, int intensity, sensor_states &sstates) {
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
        sstates.motor_left = true;
        Serial.println("Left motor enabled!");
      } else {
        sstates.motor_left = false;
        Serial.println("Left motor disabled!");
      }
    }
  } else if (event == REVENT) {
    if (intensity != sstates.ir_right) {
      Serial.println("sensor_event: right state changed!");
      sstates.ir_right = intensity;
      if (intensity == SENSOR_HIGH) {
        sstates.motor_right = true;
        Serial.println("Right motor enabled!");
      } else {
        sstates.motor_right = false;
        Serial.println("Right motor disabled!");
      }
    }
  }
}

void ir_sensor_poll(sensor_states &sstates) {
  if (digitalRead(LEYE) == HIGH) {
    ir_sensor_event(LEVENT, SENSOR_LOW, sstates);
  } else {
    ir_sensor_event(LEVENT, SENSOR_HIGH, sstates);
  }
  if (digitalRead(REYE) == HIGH) {
    ir_sensor_event(REVENT, SENSOR_LOW, sstates);
  } else {
    ir_sensor_event(REVENT, SENSOR_HIGH, sstates);
  }
}

void motor(sensor_states &sstates) {
  if (sstates.motor_left) {
    Serial.println("Left motor go!");
    digitalWrite(L_MOTOR_IN1, HIGH);
    digitalWrite(L_MOTOR_IN2, LOW);
  } else {
    digitalWrite(L_MOTOR_IN1, LOW);
    digitalWrite(L_MOTOR_IN2, LOW);
  }
  if (sstates.motor_right) {
    Serial.println("Right motor go!");
    digitalWrite(R_MOTOR_IN1, HIGH);
    digitalWrite(R_MOTOR_IN2, LOW);
  } else {
    digitalWrite(R_MOTOR_IN1, LOW);
    digitalWrite(R_MOTOR_IN2, LOW);
  }
}

void loop() {
  ir_sensor_poll(sstates);
  motor(sstates);
  delay(1000);
}
