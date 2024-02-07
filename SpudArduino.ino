const int LEYE = A1; 
const int REYE = A2;

const in2
const int R_MOTOR_IN1;
const int R_MOTOR_IN2;
const int L_MOTOR_IN1;
const int L_MOTOR_IN2;

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
      }
    }
  } else if (event == REVENT) {
    if (intensity != sstates.ir_right) {
      Serial.println("sensor_event: right state changed!");
      sstates.ir_right = intensity;
      if (intensity == SENSOR_HIGH) {
        sstates.motor_right = true;
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
  if (motor_left) {
    digitalWrite(L)
  }
  if (motor_right) {
    //
  }
}

void loop() {
  ir_sensor_poll(sstates);
  motor(sstates);
  delay(1000);
}
