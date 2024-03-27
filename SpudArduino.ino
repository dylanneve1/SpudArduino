// Includes
#include "SensorManager.h"
#include "SpudArduino.h"
#include "WiFiManager.h"

// Class initialization
SensorManager sensors;
WiFiManager wifi;

// Data structure of states
sensor_states sstates;
arduino_states astates;

// Flag to control the motors
bool motorsEnabled = false;

// Check if US has been polled
bool firstPoll = true;

// Current state of buggy
// work == BUGGY_IDLE means off
// work == BUGGY_WORK means on
int work = BUGGY_IDLE;

// Encoder revolution count
volatile int leftRevolutions = 0;
volatile int rightRevolutions = 0;

// Setup function
void setup() {
  // Set start time, last update time
  // and last server communication time
  // to the current millis()
  astates.start_time = millis();
  astates.last_update_time = millis();
  astates.last_server_time = millis();
  astates.last_distance_time = millis();
  astates.last_speed_calc_time = millis();
  astates.last_pid_calc_time = millis();
  // Begin serial connection
  Serial.begin(9600);
  // Setup pins
  sensors.pinSetup();
  // Wireless Setup
  // - Setup access point
  // - Connect server
  wifi.setupAP();
  wifi.setupServer();
  // Setup interrupts for encoders
  attachInterrupt(digitalPinToInterrupt(L_MOTOR_ENC), LencoderISR, RISING);
  attachInterrupt(digitalPinToInterrupt(R_MOTOR_ENC), RencoderISR, RISING);
}

// Main loop
void loop() {
  astates.dist = sensors.checkWheelEnc(leftRevolutions, rightRevolutions);
  // Check whether buggy has recieved
  // start or stop command and if it
  // has recieved the stop command then
  // disable the motors, we don't need
  // to re-enable them because the SensorManager
  // will audiomatically do that
  work = wifi.startStopCommandReceived();
  if (work != BUGGY_WORK) {
    sensors.changeMotor(LEFT_MOTOR_DISABLE, sstates);
    sensors.changeMotor(RIGHT_MOTOR_DISABLE, sstates);
  }
  // Set the current time before starting loop
  astates.current_time = millis();
  // Sensors
  sensors.probe(work, sstates, astates);
  if (millis() - astates.last_server_time >= SERVER_POLL_TIMEFRAME) {
    // Print current information
    printCurrentInfo();
    // Check for start/stop command from Processing
    astates.last_server_time = millis();
  }
  if (millis() - astates.last_speed_calc_time >= 1000) {
    if (astates.first_distance_checked) {
      double ret = checkAvgSpeed();
      if (ret != NULL && ret > 0) {
        astates.avg_v = ret;
      }
    } else {
      astates.avg_v = astates.dist / ((astates.current_time - astates.start_time) / 1000);
      astates.last_distance_time = astates.current_time;
      astates.first_distance_checked = true;
    }
    astates.last_speed_calc_time = astates.current_time;
  }
}

void LencoderISR() {
  leftRevolutions++;
}

void RencoderISR() {
  rightRevolutions++;
}

void printCurrentInfo() {
  String data = "L:";
  data += sstates.left_motor_speed;
  data += ",R:";
  data += sstates.right_motor_speed;
  data += ",D:";
  data += String(sensors.getUltrasonicDistance());
  data += ",T:";
  data += astates.dist;
  wifi.messageClient(data);
}

double checkAvgSpeed() {
  double delta_dist = astates.dist - astates.last_dist;
  unsigned int delta_time = astates.current_time - astates.last_distance_time;
  delta_time = delta_time / 1000;
  double ret = delta_dist / delta_time;
  astates.last_dist = astates.dist;
  astates.last_distance_time = astates.current_time;
  ret *= 100;
  return ret;
}
