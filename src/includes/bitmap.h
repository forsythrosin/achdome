#pragma once

#include <pixelValue.h>

class Bitmap {
 public:
  Bitmap(int width, int height, PixelValue value);
  ~Bitmap();
  void setPixel(int x, int y, PixelValue value);
  PixelValue getPixel(int x, int y);
  int **getData();
  void clear(PixelValue value);
 private:
  PixelValue *values;
  int width;
  int height;
};
