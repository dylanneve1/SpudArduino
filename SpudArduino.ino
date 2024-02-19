// Includes
#include "SensorManager.h"
#include "MotorManager.h"
#include "SpudArduino.h"
#include "GuidanceManager.h"

// Class initialization
SensorManager sensors;
MotorManager motors;
GuidanceManager guidance;

// Data structure of states
sensor_states sstates;
motor_states mstates;
arduino_states astates;

// Setup function
void setup() {
  // Set to start millis
  astates.start_time = millis();
  // Begin serial connection
  Serial.begin(9600);
  // Setup pins
  sensors.pinSetup();
  motors.pinSetup();
}

// Main loop
void loop() {
  // Refresh the current hits
  guidance.refresh(mstates);
  // Serial.print("Current hits in last ");
  // Serial.print(HIT_TIMEFRAME);
  // Serial.print(" is ");
  // Serial.println(guidance.poll());
  // Set the current time before starting loop
  astates.current_time = millis();
  // Probe devices passing them states
  // incase changes have occurred
   // Check distance with ultrasonic sensor
  int distance = getUltrasonicDistance();

  Serial.print("Distance detected: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance < 5.0) {
    Serial.println("YOU NEED TO STOP!");
    stopMotors();
  } else {
    // Update sensors and motors if a certain time has passed since the last update
    if (astates.current_time - astates.last_update_time >= UPDATE_INTERVAL) {
      sensors.probe(sstates, mstates, guidance);
      motors.probe(mstates);
      astates.last_update_time = astates.current_time;  // Update last update time
    }
  }
}
  // Function to stop motors
void stopMotors() {
  digitalWrite(L_MOTOR_IN1, LOW);
  digitalWrite(L_MOTOR_IN2, LOW);
  digitalWrite(R_MOTOR_IN1, LOW);
  digitalWrite(R_MOTOR_IN2, LOW);
}

// Function to get ultrasonic distance
int getUltrasonicDistance() {
  digitalWrite(US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_TRIG, LOW);
  long duration = pulseIn(US_ECHO, HIGH);
  return duration / 58;
}
  //delay(DELAY_TIME);
