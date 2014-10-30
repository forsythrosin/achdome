#include <tween.h>

Tween::Tween(const double duration, const std::function<void (float t)> onTick, const std::function<void ()> onDone) :
  duration(duration), onTick(onTick), onDone(onDone) {
}


double Tween::getDuration() {
  return duration;
}

void Tween::tick(double t) {
  onTick(t);
}

void Tween::done() {
  onDone();
}
