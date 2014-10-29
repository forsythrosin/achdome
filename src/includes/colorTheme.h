#pragma once
#include <glm/glm.hpp>

class ColorTheme {
public:
  virtual ~ColorTheme() {};
  virtual glm::vec4 playerColor(int) = 0;
  virtual glm::vec4 baseColor(int) = 0;
};
