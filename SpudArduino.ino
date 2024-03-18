#include <WiFiS3.h>
#include <iostream>
#include <sstream>
#include <string>
#include <regex>

// Timeframes
#define SERVER_POLL_TIMEFRAME 2000
#define US_POLL_TIMEFRAME 500
#define SPEED_CALC_POLL_TIMEFRAME 1000

// Buggy working or idle
#define BUGGY_WORK 1
#define BUGGY_IDLE 0

// Max and min motor pwm values
// Range is currently 40
#define MOTOR_SPEED_MAX 120
#define MOTOR_SPEED_BASE 80
#define MOTOR_SPEED_NONE 0

// Control modes
#define REFERENCE_OBJECT_CONTROL 4
#define REFERENCE_SPEED_CONTROL 0

// Sensor constants
#define SENSOR_LOW 0
#define SENSOR_HIGH 1
#define LEVENT 11
#define REVENT 12

// Motor commands
#define LEFT_MOTOR_ENABLE_CMD 0
#define RIGHT_MOTOR_ENABLE_CMD 1
#define LEFT_MOTOR_DISABLE_CMD 2
#define RIGHT_MOTOR_DISABLE_CMD 3

// Encoder Pins
#define L_MOTOR_ENC 3
#define R_MOTOR_ENC 2

// IR Sensor Pins
#define LEYE 4
#define REYE 13

// Ultrasonic Pins
#define US_TRIG 8
#define US_ECHO 9

// Motor Control Pins
#define R_MOTOR_IN1 6
#define R_MOTOR_IN2 5
#define L_MOTOR_IN1 11
#define L_MOTOR_IN2 12
#define L_MOTOR_EN 10
#define R_MOTOR_EN 7

// Encoder wheel revolution distance
#define REVOLUTION_DISTANCE 0.204

// Data structures
struct sensor_states {
  int ir_left = SENSOR_LOW;
  int ir_right = SENSOR_LOW;
  int left_motor_speed = MOTOR_SPEED_BASE;
  int right_motor_speed = MOTOR_SPEED_BASE;
  int reference_speed;
  int control_mode = REFERENCE_OBJECT_CONTROL;
};

struct arduino_states {
  unsigned long start_time = 0;
  unsigned long current_time = 0;
  unsigned long last_update_time = 0;
  unsigned long last_server_time = 0;
  unsigned long last_distance_time = 0;
  unsigned long last_speed_calc_time = 0;
  bool first_distance_checked = false;
  double dist = 0;
  double last_dist = 0;
  double avg_v = 0;
};

// Global variables
sensor_states sstates;
arduino_states astates;
WiFiServer server(5200);
WiFiClient client;
volatile int leftRevolutions = 0;
volatile int rightRevolutions = 0;
int work = BUGGY_IDLE;
bool firstPoll = true;

// Function prototypes
void setup();
void loop();
void LencoderISR();
void RencoderISR();
void ir_sensor_poll();
void ir_sensor_event(int event, int intensity);
void ultrasonic_poll();
int getUltrasonicDistance();
void changeMotor(int motor);
double checkWheelEnc();
double checkAvgSpeed();
void printCurrentInfo();
int startStopCommandReceived();

void setup() {
  // Initialization
  Serial.begin(9600);
  WiFi.beginAP("SpudArduino", "Arduino2024");
  server.begin();
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
  attachInterrupt(digitalPinToInterrupt(L_MOTOR_ENC), LencoderISR, RISING);
  attachInterrupt(digitalPinToInterrupt(R_MOTOR_ENC), RencoderISR, RISING);

  // Initial states
  astates.start_time = millis();
  astates.last_update_time = millis();
  astates.last_server_time = millis();
}

void loop() {
  // Update state variables
  astates.current_time = millis();

  // IR sensor polling
  ir_sensor_poll();

  // Ultrasonic sensor polling (if enabled)
  if (millis() - astates.last_update_time >= US_POLL_TIMEFRAME || firstPoll) {
    ultrasonic_poll();
    astates.last_update_time = millis();
  }
  firstPoll = false;


  // Server communication
  if (millis() - astates.last_server_time >= SERVER_POLL_TIMEFRAME) {
    work = startStopCommandReceived();
    if (work == BUGGY_WORK) {
      Serial.print("Current distance travelled: ");
      Serial.println(astates.dist);
    }
    printCurrentInfo();
    astates.last_server_time = millis();
  }

  if (millis() - astates.last_speed_calc_time >= SPEED_CALC_POLL_TIMEFRAME) {
    // Avg speed
    // Wheel encoder distance and avg speed calculation
    astates.dist = checkWheelEnc();
    if (!astates.first_distance_checked) {
      astates.avg_v = checkAvgSpeed();
    } else {
      astates.avg_v = astates.dist / ((millis() - astates.start_time) / 1000);
      astates.first_distance_checked = true;
      astates.last_distance_time = millis();
    }
    if (sstates.control_mode == REFERENCE_SPEED_CONTROL) {
      alignBuggySpeed();
      writeMotorSpeed();
      Serial.print("Current work: ");
      Serial.println(work);
      Serial.print("Current reference speed: ");
      Serial.println(sstates.reference_speed);
      Serial.print("Average measured speed: ");
      Serial.println(astates.avg_v);
    }
    astates.last_speed_calc_time = millis();
  }
}

// *** Encoder Interrupt Service Routines (ISRs) ***

void LencoderISR() {
  leftRevolutions++;
}

void RencoderISR() {
  rightRevolutions++;
}

// *** Sensor Functions ***

void ir_sensor_poll() {
  if (work == BUGGY_WORK) {  // Only poll IR sensors if buggy is working
    if (digitalRead(LEYE) != HIGH) {
      ir_sensor_event(LEVENT, SENSOR_LOW);
    } else {
      ir_sensor_event(LEVENT, SENSOR_HIGH);
    }

    if (digitalRead(REYE) != HIGH) {
      ir_sensor_event(REVENT, SENSOR_LOW);
    } else {
      ir_sensor_event(REVENT, SENSOR_HIGH);
    }
  }
}

void ir_sensor_event(int event, int intensity) {
  if (event == LEVENT) {
    if (intensity != sstates.ir_left) {
      Serial.println("sensor_event: left state changed!");
      sstates.ir_left = intensity;
      changeMotor(intensity == SENSOR_HIGH ? LEFT_MOTOR_ENABLE_CMD : LEFT_MOTOR_DISABLE_CMD);
    }
  } else if (event == REVENT) {
    if (intensity != sstates.ir_right) {
      Serial.println("sensor_event: right state changed!");
      sstates.ir_right = intensity;
      changeMotor(intensity == SENSOR_HIGH ? RIGHT_MOTOR_ENABLE_CMD : RIGHT_MOTOR_DISABLE_CMD);
    }
  }
}

void ultrasonic_poll() {
  if (work == BUGGY_WORK) {
    int distance = getUltrasonicDistance();

    if (distance < 20.0) {
      Serial.print("Distance detected: ");
      Serial.print(distance);
      Serial.println(" cm");
      Serial.println("YOU NEED TO STOP!");
      changeMotor(LEFT_MOTOR_DISABLE_CMD);
      changeMotor(RIGHT_MOTOR_DISABLE_CMD);
    } else if (sstates.control_mode == REFERENCE_OBJECT_CONTROL) {
      // Adjust motor speeds based on distance to reference object
      if (distance > 10 && distance < 40) {
        sstates.left_motor_speed = MOTOR_SPEED_BASE + distance / 2;
        sstates.right_motor_speed = MOTOR_SPEED_BASE + distance / 2;
      } else {
        sstates.left_motor_speed = MOTOR_SPEED_MAX;
        sstates.right_motor_speed = MOTOR_SPEED_MAX;
      }
      // Enable motors based on IR sensor readings
      if (sstates.ir_left == SENSOR_HIGH) {
        changeMotor(LEFT_MOTOR_ENABLE_CMD);
      }
      if (sstates.ir_right == SENSOR_HIGH) {
        changeMotor(RIGHT_MOTOR_ENABLE_CMD);
      }
    }
  }
}

int getUltrasonicDistance() {
  digitalWrite(US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_TRIG, LOW);
  long duration = pulseIn(US_ECHO, HIGH);
  return duration / 58;
}

void changeMotor(int motor) {
  if (motor == LEFT_MOTOR_ENABLE_CMD) {
    analogWrite(L_MOTOR_EN, sstates.left_motor_speed);
    digitalWrite(L_MOTOR_IN1, HIGH);
    digitalWrite(L_MOTOR_IN2, LOW);
  } else if (motor == RIGHT_MOTOR_ENABLE_CMD) {
    analogWrite(R_MOTOR_EN, sstates.right_motor_speed);
    digitalWrite(R_MOTOR_IN1, HIGH);
    digitalWrite(R_MOTOR_IN2, LOW);
  } else if (motor == LEFT_MOTOR_DISABLE_CMD) {
    digitalWrite(L_MOTOR_IN1, LOW);
    digitalWrite(L_MOTOR_IN2, LOW);
    sstates.left_motor_speed = MOTOR_SPEED_NONE;
  } else if (motor == RIGHT_MOTOR_DISABLE_CMD) {
    digitalWrite(R_MOTOR_IN1, LOW);
    digitalWrite(R_MOTOR_IN2, LOW);
    sstates.right_motor_speed = MOTOR_SPEED_NONE;
  }
}

void writeMotorSpeed() {
  analogWrite(L_MOTOR_EN, sstates.left_motor_speed);
  analogWrite(R_MOTOR_EN, sstates.right_motor_speed);
}

// *** Wheel Encoder Functions ***

double checkWheelEnc() {
  double ret = (REVOLUTION_DISTANCE / 2) * leftRevolutions;
  ret += (REVOLUTION_DISTANCE / 2) * rightRevolutions;
  return ret * 0.25;  // Multiply by 0.25 to average and likely get a more accurate distance
}

double checkAvgSpeed() {
  double delta_dist = astates.dist - astates.last_dist;
  unsigned int delta_time = millis() - astates.last_distance_time;
  delta_time = delta_time / 1000;
  double ret = delta_dist / delta_time;
  astates.last_dist = astates.dist;
  astates.last_distance_time = millis();
  ret *= 100;
  return ret;
}

// *** Communication Functions ***

void printCurrentInfo() {
  String data = "L:";
  data += sstates.left_motor_speed;
  data += ",R:";
  data += sstates.right_motor_speed;
  data += ",D:";
  data += String(getUltrasonicDistance());
  data += ",T:";
  data += astates.dist;
  client = server.available();
  if (client.connected()) {
    client.write(data.c_str());
    client.write("\n");
  }
}

int startStopCommandReceived() {
  //return 1;
  if (client.available()) {
    //std::string command = "B:1,M:4,S:100";//client.readStringUntil('\n');
    String command_e = client.readStringUntil('\n');
    std::string command = command_e.c_str();
    std::regex pattern(R"(B:(\d+),M:(\d+),S:(\d+))");
    std::smatch match;
    if (std::regex_match(command, match, pattern)) {
      work = std::stoi(match[1]);
      sstates.control_mode = std::stoi(match[2]);
      sstates.reference_speed = std::stoi(match[3]);
    }
    if (work == 0) {
      changeMotor(LEFT_MOTOR_DISABLE_CMD);
      changeMotor(RIGHT_MOTOR_DISABLE_CMD);
    }
    return work;
  } else {
    return work;
  }
}

void alignBuggySpeed() {
  if (astates.avg_v > sstates.reference_speed) {
    sstates.left_motor_speed -= 5;
    sstates.right_motor_speed -= 5;
  } else {
    sstates.left_motor_speed += 5;
    sstates.right_motor_speed += 5;
  }
}
