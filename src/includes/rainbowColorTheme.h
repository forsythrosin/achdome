#pragma once

#include "colorTheme.h"
#include <random>

class RainbowColorTheme : public ColorTheme {
public:
  RainbowColorTheme();
  ~RainbowColorTheme();
  glm::vec3 playerColor(int);
  glm::vec3 baseColor(int);
private:
  std::mt19937 randGen;
  int hues[16];
  void setupHues();
};