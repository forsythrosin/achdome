#pragma once
#include <glm/glm.hpp>

class ColorTheme {
public:
  virtual ~ColorTheme() {};
  virtual glm::vec3 playerColor(int) = 0;
  virtual glm::vec3 baseColor(int) = 0;
};
