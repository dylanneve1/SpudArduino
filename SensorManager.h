#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

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

#define R_MOTOR_IN1    6
#define R_MOTOR_IN2    5
#define L_MOTOR_IN1   11
#define L_MOTOR_IN2   12

// Enable pins for HBridge motors
// can control voltage delivered
// and as such the speed
#define L_MOTOR_EN    10
#define R_MOTOR_EN     7

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
  void probe(int work, sensor_states &sstates);
  void pinSetup(); 
  void ultrasonic_poll(int work, sensor_states &sstates);
  int getUltrasonicDistance();
  void changeMotor(int motor);
  void wifi_poll(int work,sensor_states &sstates);
  

private:
  void ir_sensor_poll(sensor_states &sstates);
  void ir_sensor_event(int event, int intensity, sensor_states &sstates);
  
};

#endif