#include "SpudArduino.h"

sensor_states sstates;
arduino_states astates;
WiFiServer server(5200);
WiFiClient client;

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
  astates.last_update_time = astates.start_time;
  astates.last_server_time = astates.start_time;
}

void loop() {
  // Update state variables
  astates.current_time = millis();

  // IR sensor polling
  if (astates.work == BUGGY_WORK) {
    ir_sensor_poll();

    // Ultrasonic sensor polling
    if (astates.current_time - astates.last_update_time >= US_POLL_TIMEFRAME || astates.fp) {
      ultrasonic_poll();
      astates.last_update_time = astates.current_time;
      if (astates.fp) {
        astates.fp = false;
      }
    }
  }

  if (astates.current_time - astates.last_speed_calc_time >= SPEED_CALC_POLL_TIMEFRAME) {
    astates.dist = checkWheelEnc();
    if (!astates.first_distance_checked) {
      astates.avg_v = checkAvgSpeed();
      astates.first_distance_checked = true;
    } else {
      astates.avg_v = astates.dist / ((astates.current_time - astates.start_time) / 1000);
      astates.last_distance_time = astates.current_time;
    }
    astates.last_speed_calc_time = astates.current_time;
  }

  // Server communication
  if (astates.current_time - astates.last_server_time >= SERVER_POLL_TIMEFRAME) {
    astates.work = startStopCommandReceived();
    if (astates.work == BUGGY_WORK) {
      Serial.print("Current distance travelled: ");
      Serial.println(astates.dist);
    } else {
      changeMotor(LEFT_MOTOR_DISABLE_CMD);
      changeMotor(RIGHT_MOTOR_DISABLE_CMD);
    }
    printCurrentInfo();
    astates.last_server_time = astates.current_time;
  }
}

// *** Encoder Interrupt Service Routines (ISRs) ***

void LencoderISR() {
  sstates.leftRevolutions++;
}

void RencoderISR() {
  sstates.rightRevolutions++;
}

// *** Sensor Functions ***

void ir_sensor_poll() {
  ir_sensor_event(LEVENT, digitalRead(LEYE) == HIGH ? SENSOR_HIGH : SENSOR_LOW);
  ir_sensor_event(REVENT, digitalRead(REYE) == HIGH ? SENSOR_HIGH : SENSOR_LOW);
}

void ir_sensor_event(int event, int intensity) {
  if (event == LEVENT && intensity != sstates.ir_left) {
    Serial.println("sensor_event: left state changed!");
    sstates.ir_left = intensity;
    changeMotor(intensity == SENSOR_HIGH ? LEFT_MOTOR_ENABLE_CMD : LEFT_MOTOR_DISABLE_CMD);
  } else if (event == REVENT && intensity != sstates.ir_right) {
    Serial.println("sensor_event: right state changed!");
    sstates.ir_right = intensity;
    changeMotor(intensity == SENSOR_HIGH ? RIGHT_MOTOR_ENABLE_CMD : RIGHT_MOTOR_DISABLE_CMD);
  }
}

void ultrasonic_poll() {
  int distance = getUltrasonicDistance();

  if (distance < 20.0) {
    Serial.print("Distance detected: ");
    Serial.print(distance);
    Serial.println(" cm");
    Serial.println("YOU NEED TO STOP!");
    changeMotor(LEFT_MOTOR_DISABLE_CMD);
    changeMotor(RIGHT_MOTOR_DISABLE_CMD);
  } else {
    // Enable motors based on IR sensor readings
    if (sstates.ir_left == SENSOR_HIGH) {
      changeMotor(LEFT_MOTOR_ENABLE_CMD);
    }
    if (sstates.ir_right == SENSOR_HIGH) {
      changeMotor(RIGHT_MOTOR_ENABLE_CMD);
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

// *** Wheel Encoder Functions ***

double checkWheelEnc() {
  double ret = (REVOLUTION_DISTANCE / 2) * sstates.leftRevolutions;
  ret += (REVOLUTION_DISTANCE / 2) * sstates.rightRevolutions;
  return ret * 0.25;
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
  data += ",S:";
  data += String(astates.avg_v);
  client = server.available();
  if (client.connected()) {
    client.write(data.c_str());
    client.write("\n");
  }
}

int startStopCommandReceived() {
  if (client.available()) {
    String command_e = client.readStringUntil('\n');
    std::string command = command_e.c_str();
    std::regex pattern(R"(B:(\d+),S:(\d+))");
    std::smatch match;
    if (std::regex_match(command, match, pattern)) {
      astates.work = std::stoi(match[1]);
      astates.reference_speed = std::stoi(match[2]);
    }
    if (astates.work == BUGGY_IDLE) {
      changeMotor(LEFT_MOTOR_DISABLE_CMD);
      changeMotor(RIGHT_MOTOR_DISABLE_CMD);
    } else {
      // Enable motors based on IR sensor readings
      if (sstates.ir_left == SENSOR_HIGH) {
        changeMotor(LEFT_MOTOR_ENABLE_CMD);
      }
      if (sstates.ir_right == SENSOR_HIGH) {
        changeMotor(RIGHT_MOTOR_ENABLE_CMD);
      }
    }
    return astates.work;
  } else {
    return astates.work;
  }
}
