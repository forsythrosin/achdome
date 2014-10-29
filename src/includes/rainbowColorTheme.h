#pragma once

#include "colorTheme.h"
#include <random>

class RainbowColorTheme : public ColorTheme {
public:
  RainbowColorTheme();
  ~RainbowColorTheme();
  glm::vec4 playerColor(int);
  glm::vec4 baseColor(int);
private:
  std::mt19937 randGen;
  int hues[16];
  void setupHues();
};