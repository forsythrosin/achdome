#include "rainbowColorTheme.h"
#include <glm/ext.hpp>

RainbowColorTheme::RainbowColorTheme() {
  std::random_device rd;
  randGen = std::mt19937(rd());
  setupHues();
}

RainbowColorTheme::~RainbowColorTheme() {

}

glm::vec3 RainbowColorTheme::playerColor(int i) {
  // Generate color in HSV space and convert to RGB
  std::uniform_real_distribution<float> vDis(0.0, 0.1);
  float v = 0.9f + vDis(randGen); // [0.9,1.0[
  float s = 0.4f + (1 - v) * 0.3f; // [0.4,0.7[ : Linearly dependent on v
  int h = hues[i % 16];

  return glm::rgbColor(glm::vec3(h, s, v));
}

glm::vec3 RainbowColorTheme::baseColor(int i) {
  switch (i) {
  case 0:
    return glm::vec3(0.0, 0.0, 0.0);
  case 1:
    return glm::vec3(0.3, 0.3, 0.3);
  }
  return glm::vec3(0.0, 0.0, 0.0);
}

void RainbowColorTheme::setupHues() {
  hues[0] = 336;
  hues[1] = 160;
  hues[2] = 45;
  hues[3] = 215;
  hues[4] = 20;
  hues[5] = 265;
  hues[6] = 80;
  hues[7] = 0;
  hues[8] = 180;
  hues[9] = 240;
  hues[10] = 60;
  hues[11] = 190;
  hues[12] = 130;
  hues[13] = 300;
  hues[14] = 225;
  hues[15] = 100;
}