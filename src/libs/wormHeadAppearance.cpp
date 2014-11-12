#include <wormHeadAppearance.h>
#include <tweener.h>
#include <tween.h>

WormHeadAppearance::WormHeadAppearance(glm::vec4 color, float diameterCoefficient, float strokeWidth, float arrowLength, float arrowWidth) {
  this->color = color;
  this->diameterCoefficient = diameterCoefficient;
  this->strokeWidth = strokeWidth;
  this->arrowLength = arrowLength;
  this->arrowWidth = arrowWidth;
}

float WormHeadAppearance::getDiameterCoefficient() {
  return diameterCoefficient;
}

float WormHeadAppearance::getStrokeWidth() {
  return strokeWidth;
}

float WormHeadAppearance::getArrowLength() {
  return arrowLength;
}

float WormHeadAppearance::getArrowWidth() {
  return arrowWidth;
}

glm::vec4 WormHeadAppearance::getColor() {
  return color;
}

void WormHeadAppearance::setDiameterCoefficient(float dc) {
  diameterCoefficient = dc;
}

void WormHeadAppearance::setStrokeWidth(float sw) {
  strokeWidth = sw;
}

void WormHeadAppearance::setArrowLength(float al) {
  arrowLength = al;
}

void WormHeadAppearance::setArrowWidth(float aw) {
  arrowWidth = aw;
}

void WormHeadAppearance::setColor(glm::vec4 c) {
  color = c;
}

void WormHeadAppearance::tweenDiameterCoefficient(float destination, float duration) {
  // stop previous tween if any.
  Tweener::getInstance()->stopTween(diameterTween);

  float origin = diameterCoefficient;
  Tween tween = Tween(duration, [this, destination, origin](double t) {
        this->diameterCoefficient = origin*(1 - t) + destination*t;
      }, []() {
        // do nothing when done.
    });
  diameterTween = Tweener::getInstance()->startTween(tween);
}

void WormHeadAppearance::tweenStrokeWidth(float destination, float duration) {
  // stop previous tween if any.
  Tweener::getInstance()->stopTween(strokeWidthTween);

  float origin = strokeWidth;
  Tween tween = Tween(duration, [this, destination, origin](double t) {
      this->strokeWidth = origin*(1 - t) + destination*t;
      }, []() {
        // do nothing when done.
    });
  strokeWidthTween = Tweener::getInstance()->startTween(tween);
}


void WormHeadAppearance::tweenArrowWidth(float destination, float duration) {
  // stop previous tween if any.
  Tweener::getInstance()->stopTween(arrowWidthTween);

  float origin = arrowWidth;
  Tween tween = Tween(duration, [this, destination, origin](double t) {
      this->arrowWidth = origin*(1 - t) + destination*t;
      }, []() {
        // do nothing when done.
    });
  arrowWidthTween = Tweener::getInstance()->startTween(tween);
}

void WormHeadAppearance::tweenArrowLength(float destination, float duration) {
  // stop previous tween if any.
  Tweener::getInstance()->stopTween(arrowLengthTween);

  float origin = arrowLength;
  Tween tween = Tween(duration, [this, destination, origin](double t) {
      this->arrowLength = origin*(1 - t) + destination*t;
      }, []() {
        // do nothing when done.
    });
  arrowLengthTween = Tweener::getInstance()->startTween(tween);
}
