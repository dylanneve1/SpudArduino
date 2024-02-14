#include "GuidanceManager.h"
#include <Serial.h>

void GuidanceManager::createEntry() {
  hits[current_len] = millis();
  current_len++;
}

void GuidanceManager::refresh() {
  for (int i = 0; i < current_len; i++) {
    unsigned long timediff = millis() - hits[i];
    if (timediff >= HIT_TIMEFRAME) {
      current_len--;
      return;
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
