class Bitmap {
 public:
  Bitmap(int width, int height, int value);
  ~Bitmap();
  void setPixel(int x, int y, int value);
  int getPixel(int x, int y);
  int **getData();
  void clear(int value);
 private:
  int *values;
  int width;
  int height;
};
