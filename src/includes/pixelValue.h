#pragma once

struct PixelValue {
public:
  PixelValue();
  PixelValue(int wormId, int time);
  int wormId;
  int time;
  bool isEmpty();
  bool isOutsideBounds();
  static PixelValue createEmpty();
  static PixelValue createOutsideBounds();
  bool operator==(PixelValue pv);
};


