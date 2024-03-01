#include "VisionManager.h"

VisionManager::VisionManager(HUSKYLENS* lens, SensorManager* sensors) {
  this->lens = lens;
  this->sensors = sensors;
}

bool VisionManager::VisionSetup() {
  Wire.begin();
  for (int i = 0; i < 5; i++) {
    if (!lens->begin(Wire)) {
      Serial.println("VisionManager: not connected... retrying in 1000ms...");
      delay(1000);
    } else {
      Serial.println("VisionManager: HuskyLens connected successfully!");
    }
  }
}

int VisionManager::FollowTrainedObject() {
  int ret;
  if (!lens->request()) {
    ret = LENS_REQUEST_FAILED;
    if (ObjectLock) {
      Serial.println("VisionManager: lost sight of target!");
      ObjectLock = false;
    }
  } else if(!lens->isLearned()) {
    ret = LENS_REQUEST_NOT_LEARNED;
    if (ObjectLock) {
      Serial.println("VisionManager: lost sight of target!");
      ObjectLock = false;
    }
  } else if(!lens->available()) {
    ret = LENS_REQUEST_DISSAPEARED;
    if (ObjectLock) {
      Serial.println("VisionManager: lost sight of target!");
      ObjectLock = false;
    }
  } else {
    ret = LENS_REQUEST_SUCCESS;
    if (!ObjectLock) {
      Serial.println("VisionManager: locked on to target!");
      ObjectLock = true;
    }
    HUSKYLENSResult result = lens->read();
    AdjustMotors(result.xOrigin);
  }
  return ret;
}

void VisionManager::AdjustMotors(double horizontal) {
  if (horizontal < 150) {
    sensors->changeMotor(LEFT_MOTOR_DISABLE);
    sensors->changeMotor(RIGHT_MOTOR_ENABLE);
  } else if (horizontal > 250) {
    sensors->changeMotor(LEFT_MOTOR_ENABLE);
    sensors->changeMotor(RIGHT_MOTOR_DISABLE);
  } else {
    sensors->changeMotor(LEFT_MOTOR_ENABLE);
    sensors->changeMotor(RIGHT_MOTOR_ENABLE);
  }
}
