#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "MotorManager.h"
#include "GuidanceManager.h"

// IR sensor events
#define LEVENT       11
#define REVENT       12

// IR sensor states
#define SENSOR_LOW   13
#define SENSOR_HIGH  14

// IR sensor pins
#define LEYE          4 
#define REYE         13

//define ultrasonic pins

#define US_TRIG      8
#define US_ECHO      9

// Data structure to store IR sensor
// states between loops, we will default
// to LOW state
struct sensor_states {
  int ir_left = SENSOR_LOW;
  int ir_right = SENSOR_LOW;
};

// SensorManager class
class SensorManager {

public:
  void probe(sensor_states &sstates, motor_states &mstates, GuidanceManager &guidance);
  void pinSetup();
  


private:
  void ir_sensor_poll(sensor_states &sstates, motor_states &mstates, GuidanceManager &guidance);
  void ir_sensor_event(int event, int intensity, sensor_states &sstates, motor_states &mstates, GuidanceManager &guidance);
  
};

#endif