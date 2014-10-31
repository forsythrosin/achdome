#pragma once
#include <functional>

class Tween {
 public:
  Tween(double duration, const std::function<void (float t)> onTick, const std::function<void ()> onDone);
  double getDuration();
  void tick(double t);
  void done();
 private:
  const double duration;
  std::function<void (float t)> onTick;
  std::function<void ()> onDone;
};


