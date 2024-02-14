#ifndef GUIDANCE_MANAGER_H
#define GUIDANCE_MANAGER_H

#define HIT_TIMEFRAME 5000

// GuidanceManager class
class GuidanceManager {
private:
  int current_len = 0;
  unsigned long hits[100];
public:
  void refresh();
  void createEntry();
  int poll();
};

#endif
