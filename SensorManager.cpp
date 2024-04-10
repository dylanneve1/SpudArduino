#include "SensorManager.h"
#include <Serial.h>
#include "WiFiManager.h"
#include "SpudArduino.h"
#include "frames.h"

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
    if (sstates.firstPoll) {
      matrix.begin();
    }
    ir_sensor_poll(sstates, astates);
    if (millis() - astates.last_update_time >= US_POLL_TIMEFRAME || sstates.firstPoll) {
      ultrasonic_poll(work, sstates, astates);
      astates.last_update_time = millis();
      if (sstates.firstPoll) {
        sstates.firstPoll = false;
      }
    }
  } else if (!sstates.firstPoll) {
    matrix.renderBitmap(off_frame, 8, 12);
  }
}

void SensorManager::ir_sensor_poll(sensor_states &sstates, arduino_states &astates) {
  int left_intensity = digitalRead(LEYE) != HIGH ? SENSOR_LOW : SENSOR_HIGH;
  int right_intensity = digitalRead(REYE) != HIGH ? SENSOR_LOW : SENSOR_HIGH;
  if (left_intensity != sstates.ir_left) {
    sstates.ir_left = left_intensity;
    if (left_intensity == SENSOR_HIGH) {
      changeMotor(LEFT_MOTOR_ENABLE, sstates, astates);
    } else if (left_intensity == SENSOR_LOW) {
      changeMotor(LEFT_MOTOR_TURN, sstates, astates);
    }
  }

  if (right_intensity != sstates.ir_right) {
    sstates.ir_right = right_intensity;
    if (right_intensity == SENSOR_HIGH) {
      changeMotor(RIGHT_MOTOR_ENABLE, sstates, astates);
    } else if (right_intensity == SENSOR_LOW) {
      changeMotor(RIGHT_MOTOR_TURN, sstates, astates);
    }
  }
}

// Function to change motor states
// between on and off for the left
// and right motors
void SensorManager::changeMotor(int motor, sensor_states &sstates, arduino_states &astates) {
  int leftSpeed, rightSpeed;
  if (!sstates.pidEnabled) {
    leftSpeed = sstates.left_motor_speed;
    rightSpeed = sstates.right_motor_speed;
  } else {
    leftSpeed = MOTOR_SPEED_PID + abs(sstates.pidCoef * PID_MULTIPLE);
    rightSpeed = MOTOR_SPEED_PID + abs(sstates.pidCoef * PID_MULTIPLE);
  }
  if (sstates.ir_left != sstates.ir_right) {
    leftSpeed = 200;
    rightSpeed = 200;
  }
  if (leftSpeed > 200 || rightSpeed > 200) {
    leftSpeed = 200;
    rightSpeed = 200;
  }
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
    int distance = getUltrasonicDistance(sstates);
    sstates.usdist = distance;
    if (distance < 20.0) {
      changeMotor(LEFT_MOTOR_DISABLE, sstates, astates);
      changeMotor(RIGHT_MOTOR_DISABLE, sstates, astates);
      sstates.pidEnabled = true;
      matrix.renderBitmap(off_frame, 8, 12);
      return;
    } else if (distance < 35.0) {
      sstates.pidCoef = computePID(distance, astates, sstates);
      sstates.pidEnabled = true;
      sstates.converted_reference_speed = INITIAL_REF_SPEED;
    } else {
      sstates.pidEnabled = false;
    }
    if (sstates.pidEnabled) {
      matrix.renderBitmap(pid_frame, 8, 12);
    } else {
      matrix.renderBitmap(obj_frame, 8, 12);
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
int SensorManager::getUltrasonicDistance(sensor_states &sstates) {
  int ret;
  digitalWrite(US_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(US_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_TRIG, LOW);
  long duration = pulseIn(US_ECHO, HIGH);
  ret = duration / 58;
  if (sstates.first_us_ret) {
    if (ret <= 20) {
      sstates.last_us_ret = ret;
      return ret;
    }
    if (abs(ret - sstates.last_us_ret) > 100) {
      if (sstates.bad_ret_cnt <= 3) {
        sstates.bad_ret_cnt++;
        return sstates.last_us_ret;
      } else {
        sstates.last_us_ret = ret;
        sstates.bad_ret_cnt = 0;
        return ret;
      }
    } else {
      return ret;
    }
  } else {
    sstates.last_us_ret = ret;
    sstates.first_us_ret = true;
    return ret;
  }
}

double SensorManager::checkWheelEnc(volatile int leftRevolutions, volatile int rightRevolutions) {
  double ret = 0;
  ret = (REVOLUTION_DISTANCE / 2) * leftRevolutions;
  ret += (REVOLUTION_DISTANCE / 2) * rightRevolutions;
  ret = ret * 0.25;
  return ret;
}

double SensorManager::computePID(double inp, arduino_states &astates, sensor_states &sstates) {
  elapsedTime = (double)(astates.current_time - astates.last_pid_calc_time);

  sstates.error = (setPoint - inp);
  cumError += sstates.error * elapsedTime;
  rateError = (sstates.error - lastError) / elapsedTime;

  double ret = kp * sstates.error + ki * cumError + kd * rateError;

  lastError = sstates.error;
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
      if (calculation < 100 && calculation >= 0) {
        astates.avg_v = calculation;
      }
    } else {
      astates.avg_v = astates.dist / ((astates.current_time - astates.start_time) / 1000);
      astates.last_distance_time = astates.current_time;
      astates.first_distance_checked = true;
    }
    astates.last_speed_calc_time = astates.current_time;
    if (!sstates.pidEnabled && sstates.ir_right == SENSOR_HIGH && sstates.ir_left == SENSOR_HIGH && sstates.usdist > 20) {
      alignBuggySpeed(sstates, astates);
    }
  }
}

void SensorManager::alignBuggySpeed(sensor_states &sstates, arduino_states &astates) {
  int newSpeed = sstates.converted_reference_speed;;
  if (sstates.reference_speed > astates.avg_v) {
    newSpeed += 10;
  } else {
    newSpeed -= 10;
  }
  if (newSpeed >= 0) {
    if (newSpeed > 200) {
      newSpeed = 200;
    }
    sstates.converted_reference_speed = newSpeed;
    sstates.left_motor_speed = newSpeed;
    sstates.right_motor_speed = newSpeed;
  }
}
