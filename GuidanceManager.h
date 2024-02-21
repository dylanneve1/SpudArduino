#ifndef GUIDANCE_MANAGER_H
#define GUIDANCE_MANAGER_H

#include "MotorManager.h"

#define HIT_TIMEFRAME 5000

// GuidanceManager class
class GuidanceManager {
private:
  int current_len = 0;
  unsigned long hits[100];
  void reorderList();
public:
  void refresh(motor_states &mstates);
  void createEntry();
  int poll();
};

#endif
