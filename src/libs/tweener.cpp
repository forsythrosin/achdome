#include <tweener.h>
#include "sgct.h"

Tweener *Tweener::instance = 0;

Tweener::Tweener() {
  nextTweenId = 0;
}


Tweener::~Tweener() {}

Tweener* Tweener::getInstance() {
  if (instance == nullptr) {
    instance = new Tweener();
  }
  return instance;
}


int Tweener::startTween(Tween tween) {
  tween.tick(0.0);
  TrackedTween *tt = new TrackedTween(tween, sgct::Engine::getTime());
  int id = nextTweenId++;
  trackedTweens[id] = tt;
  return id;
}

void Tweener::stopTween(int id) {
  if (trackedTweens.count(id) > 0) {
    TrackedTween *trackedTween = trackedTweens[id];
    delete trackedTween;
    trackedTweens.erase(id);
  }
}


void Tweener::tick() {
  double currentTime = sgct::Engine::getTime();
  for (auto it = trackedTweens.cbegin(); it != trackedTweens.cend(); /* no increment */ ) {
    TrackedTween *trackedTween = it->second;
    double startTime = trackedTween->startTime;
    Tween tween = trackedTween->tween;
    double duration = tween.getDuration();
    double t = (currentTime - startTime)/duration;

    if (t >= 1.0) {
      tween.tick(1.0);
      tween.done();
      delete trackedTween;
      trackedTweens.erase(it++);
    } else {
      tween.tick(t);
      ++it;
    }
  }
}
