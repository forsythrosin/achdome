#include <bitmap.h>
#include <pixelValue.h>
#include <fstream>

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


void Bitmap::saveToPPM(const char *filename) {
  std::ofstream ofs;
  ofs.open(filename);
  ofs << "P6\n" << width << " " << height << "\n255\n";
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      PixelValue val = getPixel(i, j);
      int id = val.wormId;
      int time = val.time;
      unsigned char r = (unsigned char) (id*183);
      unsigned char g = (unsigned char) (100 + id*128);
      unsigned char b = (unsigned char) (time*78);
      ofs << r << g << b;
    }
  }
  ofs.close();
}
