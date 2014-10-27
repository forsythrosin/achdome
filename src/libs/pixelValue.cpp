#include <pixelValue.h>

PixelValue::PixelValue() {
  this->wormId = -1;
  this->time = 0;
}

PixelValue::PixelValue(int wormId, int time) {
  this->wormId = wormId;
  this->time = time;
}

bool PixelValue::operator==(PixelValue pv) {
  return this->wormId == pv.wormId && this->time == pv.time;
}

bool PixelValue::isEmpty() {
  return *this == PixelValue::createEmpty();
}


bool PixelValue::isOutsideBounds() {
  return *this == PixelValue::createOutsideBounds();
}

PixelValue PixelValue::createEmpty() {
  return PixelValue(-1, -1);
}


PixelValue PixelValue::createOutsideBounds() {
  return PixelValue(-1, 0);
}
