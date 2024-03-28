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
void SensorManager::probe(int work, sensor_states &sstates, arduino_states &astates) {
  if (work == BUGGY_WORK) {
    ir_sensor_poll(sstates, astates);
    if (millis() - astates.last_update_time >= US_POLL_TIMEFRAME || sstates.firstPoll) {
      ultrasonic_poll(work, sstates, astates);
      astates.last_update_time = millis();
      if (sstates.firstPoll) {
        sstates.firstPoll = false;
      }
    }
  }
}

void SensorManager::ir_sensor_poll(sensor_states &sstates, arduino_states &astates) {
  if (digitalRead(LEYE) != HIGH) {
    ir_sensor_event(LEVENT, SENSOR_LOW, sstates, astates);
  } else {
    ir_sensor_event(LEVENT, SENSOR_HIGH, sstates, astates);
  }
  if (digitalRead(REYE) != HIGH) {
    ir_sensor_event(REVENT, SENSOR_LOW, sstates, astates);
  } else {
    ir_sensor_event(REVENT, SENSOR_HIGH, sstates, astates);
  }
}

// IR Sensor Event
void SensorManager::ir_sensor_event(int event, int intensity, sensor_states &sstates, arduino_states &astates) {
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
        changeMotor(LEFT_MOTOR_ENABLE, sstates, astates);
        Serial.println("Left motor enabled!");
      } else if (intensity == SENSOR_LOW) {
        changeMotor(LEFT_MOTOR_TURN, sstates, astates);
        Serial.println("Left motor disabled!");
      }
    }
  } else if (event == REVENT) {
    if (intensity != sstates.ir_right) {
      Serial.println("sensor_event: right state changed!");
      sstates.ir_right = intensity;
      if (intensity == SENSOR_HIGH) {
        changeMotor(RIGHT_MOTOR_ENABLE, sstates, astates);
        Serial.println("Right motor enabled!");
      } else if (intensity == SENSOR_LOW) {
        changeMotor(RIGHT_MOTOR_TURN, sstates, astates);
        Serial.println("Right motor disabled!");
      }
    }
  }
}

// Function to change motor states
// between on and off for the left
// and right motors
void SensorManager::changeMotor(int motor, sensor_states &sstates, arduino_states &astates) {
  int leftSpeed, rightSpeed;
  if (!sstates.pidEnabled) {
    alignBuggySpeed(sstates, astates);
    leftSpeed = sstates.left_motor_speed;
    rightSpeed = sstates.right_motor_speed;
  } else {
    leftSpeed = MOTOR_SPEED_PID + abs(sstates.pidCoef*PID_MULTIPLE);
    rightSpeed = MOTOR_SPEED_PID + abs(sstates.pidCoef*PID_MULTIPLE);
  }
  Serial.print("leftSpeed: ");
  Serial.println(leftSpeed);
  Serial.print("rightSpeed: ");
  Serial.println(rightSpeed);
  Serial.print("pidCoef: ");
  Serial.println(sstates.pidCoef);
  if (motor == LEFT_MOTOR_ENABLE) {
    analogWrite(L_MOTOR_EN, leftSpeed);
    digitalWrite(L_MOTOR_IN1, HIGH);
    digitalWrite(L_MOTOR_IN2, LOW);
  } else if (motor == RIGHT_MOTOR_ENABLE) {
    analogWrite(R_MOTOR_EN, rightSpeed);
    digitalWrite(R_MOTOR_IN1, HIGH);
    digitalWrite(R_MOTOR_IN2, LOW);
  } else if (motor == LEFT_MOTOR_DISABLE) {
    analogWrite(L_MOTOR_EN, MOTOR_SPEED_MIN);
    digitalWrite(L_MOTOR_IN1, LOW);
    digitalWrite(L_MOTOR_IN2, LOW);
  } else if (motor == RIGHT_MOTOR_DISABLE) {
    analogWrite(R_MOTOR_EN, MOTOR_SPEED_MIN);
    digitalWrite(R_MOTOR_IN1, LOW);
    digitalWrite(R_MOTOR_IN2, LOW);
  } else if (motor == LEFT_MOTOR_TURN) {
    analogWrite(L_MOTOR_EN, MOTOR_SPEED_TURN);
    digitalWrite(L_MOTOR_IN1, LOW);
    digitalWrite(L_MOTOR_IN2, HIGH);
  } else if (motor == RIGHT_MOTOR_TURN) {
    analogWrite(R_MOTOR_EN, MOTOR_SPEED_TURN);
    digitalWrite(R_MOTOR_IN1, LOW);
    digitalWrite(R_MOTOR_IN2, HIGH);
  }
}

void SensorManager::ultrasonic_poll(int work, sensor_states &sstates, arduino_states &astates) {
  // Probe devices passing them states
  // incase changes have occurred
  // Check distance with ultrasonic sensor
  if (work == BUGGY_WORK) {
    int distance = getUltrasonicDistance();

    sstates.usdist = distance;

    if (distance < 20.0) {
      Serial.print("Distance detected: ");
      Serial.print(distance);
      Serial.println(" cm");
      Serial.println("YOU NEED TO STOP!");
      changeMotor(LEFT_MOTOR_DISABLE, sstates, astates);
      changeMotor(RIGHT_MOTOR_DISABLE, sstates, astates);
      return;
    } 
    else if (distance < 75.0) {
      sstates.pidCoef = computePID(distance, astates);
      sstates.pidEnabled = true;
    } else {
      sstates.pidEnabled = false;
    }
    if (sstates.ir_left == SENSOR_HIGH) {
      changeMotor(LEFT_MOTOR_ENABLE, sstates, astates);
    }
    if (sstates.ir_right == SENSOR_HIGH) {
      changeMotor(RIGHT_MOTOR_ENABLE, sstates, astates);
    }
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

double SensorManager::computePID(double inp, arduino_states &astates) {
  elapsedTime = (double)(astates.current_time - astates.last_pid_calc_time);

  error = (setPoint - inp);
  cumError += error * elapsedTime;
  rateError = (error - lastError) / elapsedTime;

  double ret = kp * error + ki * cumError + kd * rateError;

  lastError = error;
  astates.last_pid_calc_time = astates.current_time;

  return ret;
}

void SensorManager::calculateBuggySpeed(sensor_states &sstates, arduino_states &astates) {
  if (millis() - astates.last_speed_calc_time >= 1000) {
    if (astates.first_distance_checked) {
      double delta_dist = astates.dist - astates.last_dist;
      unsigned int delta_time = astates.current_time - astates.last_distance_time;
      delta_time = delta_time / 1000;
      double calculation = delta_dist / delta_time;
      astates.last_dist = astates.dist;
      astates.last_distance_time = astates.current_time;
      calculation *= 100;
      if (calculation != NULL && calculation > 0) {
        astates.avg_v = calculation;
      }
    } else {
      astates.avg_v = astates.dist / ((astates.current_time - astates.start_time) / 1000);
      astates.last_distance_time = astates.current_time;
      astates.first_distance_checked = true;
    }
    astates.last_speed_calc_time = astates.current_time;
  }
}

void SensorManager::alignBuggySpeed(sensor_states &sstates, arduino_states &astates) {
  int convertedRefSpeed;
   if (astates.avg_v > sstates.reference_speed) {
    convertedRefSpeed += sstates.reference_speed - astates.avg_v;
   }
   if (convertedRefSpeed >= 150) {
    convertedRefSpeed = 150;
   }
   if (sstates.ir_left == SENSOR_HIGH) {
     sstates.left_motor_speed = convertedRefSpeed;
   }
   if (sstates.ir_right == SENSOR_HIGH) {
     sstates.right_motor_speed = convertedRefSpeed;
   }
}
