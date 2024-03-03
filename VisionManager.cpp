#include "VisionManager.h"

VisionManager::VisionManager(HUSKYLENS &lens) {
  this->lens = lens;
}

bool VisionManager::VisionSetup() {
  bool ret = false;
  sensors.pinSetup();
  Wire.begin();
  for (int i = 0; i < 5; i++) {
    if (!lens.begin(Wire)) {
      Serial.println("VisionManager: not connected... retrying in 1000ms...");
      delay(1000);
    } else {
      Serial.println("VisionManager: HuskyLens connected successfully!");
      lens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING);
      ret = true;
      break;
    }
  }
  return ret;
}

int VisionManager::FollowTrainedObject() {
  int ret;
  if (!lens.request()) {
    ret = LENS_REQUEST_FAILED;
    if (ObjectLock) {
      Serial.println("VisionManager: lost sight of target!");
      ObjectLock = false;
    }
    AdjustMotors(0, 1);
  } else if(!lens.isLearned()) {
    ret = LENS_REQUEST_NOT_LEARNED;
    if (ObjectLock) {
      Serial.println("VisionManager: lost sight of target!");
      ObjectLock = false;
    }
    AdjustMotors(0, 1);
  } else if(!lens.available()) {
    ret = LENS_REQUEST_DISSAPEARED;
    if (ObjectLock) {
      Serial.println("VisionManager: lost sight of target!");
      ObjectLock = false;
    }
    AdjustMotors(0, 1);
  } else {
    ret = LENS_REQUEST_SUCCESS;
    if (!ObjectLock) {
      Serial.println("VisionManager: locked on to target!");
      ObjectLock = true;
    }
    HUSKYLENSResult result = lens.read();
    AdjustMotors(result.xOrigin, 0);
  }
  return ret;
}

void VisionManager::AdjustMotors(double horizontal, int reset) {
  if (reset == 1) {
    sensors.changeMotor(LEFT_MOTOR_DISABLE);
    sensors.changeMotor(RIGHT_MOTOR_DISABLE);
    return;
  }
  if (horizontal < 150) {
    sensors.changeMotor(LEFT_MOTOR_DISABLE);
    sensors.changeMotor(RIGHT_MOTOR_ENABLE);
  } else if (horizontal > 250) {
    sensors.changeMotor(LEFT_MOTOR_ENABLE);
    sensors.changeMotor(RIGHT_MOTOR_DISABLE);
  } else {
    sensors.changeMotor(LEFT_MOTOR_ENABLE);
    sensors.changeMotor(RIGHT_MOTOR_ENABLE);
  }
}
