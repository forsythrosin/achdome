#include <bitmap.h>
#include <pixelValue.h>

Bitmap::Bitmap(int width, int height, PixelValue value) {
  this->width = width;
  this->height = height;
  values = new PixelValue[width*height];
  for (int i = 0; i < width*height; i++) {
    values[i] = value;
  }
}


Bitmap::~Bitmap() {
  delete[] values;
}

void Bitmap::setPixel(int x, int y, PixelValue value) {
  if (x >= 0 && y >= 0 && x < width && y < height) {
    values[y * width + x] = value;
  }
}


PixelValue Bitmap::getPixel(int x, int y) {
  if (x >= 0 && y >= 0 && x < width && y < height) {
    return values[y * width + x];
  }
  return PixelValue::createOutsideBounds();
}
