#include "GuidanceManager.h"
#include <Serial.h>

void GuidanceManager::createEntry() {
  Serial.println("Adding hit");
  if (current_len >= 100) {
    return;
  }
  hits[current_len] = millis();
  current_len++;
}

void GuidanceManager::refresh(motor_states &mstates) {
  for (int i = 0; i < current_len; i++) {
    unsigned long timediff = millis() - hits[i];
    if (timediff >= HIT_TIMEFRAME) {
      current_len--;
      return;
    }
  }
  int newspeed = 215 - poll()*10;
  mstates.left_speed = newspeed;
  mstates.right_speed = newspeed;
  mstates.right_needs_update = true;
  mstates.left_needs_update = true;
}

int GuidanceManager::poll() {
  int ret = 0;
  for (int i = 0; i < current_len; i++) {
    unsigned long timediff = millis() - hits[i];
    if (timediff <= HIT_TIMEFRAME) {
      ret++;
    }
  }
  return ret;
}
