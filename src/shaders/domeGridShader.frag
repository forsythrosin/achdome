#version 330 core
#define PI 3.1415926535897932384626433832795

uniform vec4 gridColor;

in vec2 sphericalPos;

out vec4 color;

void main() {
  float positionOpacity = sphericalPos.y/(PI/2);
  color = gridColor;
  color.a *= positionOpacity;
}
