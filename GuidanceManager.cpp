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
      Serial.println("Hit removed");
      current_len--;
      return;
    }
  }
  if (poll() > 0) {
    int newspeed = 215 - poll()*5;
    if (newspeed <= 200) {
      return;
    }
    if (mstates.left_speed != newspeed) {
      mstates.left_speed = newspeed;
      mstates.left_needs_update = true;
    }
    if (mstates.right_speed != newspeed) {
      mstates.right_speed = newspeed;
      mstates.right_needs_update = true;
    }
  }
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
