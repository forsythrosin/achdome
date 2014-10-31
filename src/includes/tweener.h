#pragma once

#include <tween.h>
#include <map>

struct TrackedTween {
  TrackedTween(Tween t, double startTime) : tween(t), startTime(startTime) {};
  Tween tween;
  double startTime;
};


class Tweener {
 public:
  void startTween(Tween t);
  static Tweener* getInstance();
  void tick();
 private:
  Tweener();
  ~Tweener();
  std::map<int, TrackedTween*> trackedTweens;
  static Tweener *instance;
  int nextTweenId = 0;
};
