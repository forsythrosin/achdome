#include <bitmap.h>

Bitmap::Bitmap(int width, int height, int value) {
  this->width = width;
  this->height = height;
  values = new int[width*height];
  for (int i = 0; i < width*height; i++) {
    values[i] = value;
  }
}


Bitmap::~Bitmap() {
  delete[] values;
}

void Bitmap::setPixel(int x, int y, int value) {
  if (x >= 0 && y >= 0 && x < width && y < height) {
    values[y * width + x] = value;
  }
}


int Bitmap::getPixel(int x, int y) {
  if (x >= 0 && y >= 0 && x < width && y < height) {
    return values[y * width + x];
  }
  return -1;
}
