#version 330 core

uniform float time;

in vec4 vColor;
in vec3 center;
in vec3 worldPos;
in vec3 forwardPoint;

out vec4 color;

void main() {
  vec4 headColor = vColor;
  float distToCenter = distance(worldPos, center);
  headColor.a *= 1.0 - step(1.0, distToCenter);
  float distToFront = distance(worldPos, forwardPoint);
  headColor.r *= 1.0 - step(1.0, distToFront);
  headColor.g *= sin(time*0.1);

  color = headColor;
}
