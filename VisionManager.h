#ifndef VISION_MANAGER_H
#define VISION_MANAGER_H

#include <HUSKYLENS.h>
#include <SoftwareSerial.h>

#include "SensorManager.h"

// Huskylens
#include <Wire.h>

#define LENS_REQUEST_FAILED 24
#define LENS_REQUEST_NOT_LEARNED 25
#define LENS_REQUEST_DISSAPEARED 26
#define LENS_REQUEST_SUCCESS 27

// VisionManager class
class VisionManager {
private:
  bool ObjectLock = false;
  HUSKYLENS* lens;
  SensorManager* sensors;
  void AdjustMotors(double horizontal);
public:
  VisionManager(HUSKYLENS* lens, SensorManager* sensors);
  bool VisionSetup();
  int FollowTrainedObject();
};

#endif
