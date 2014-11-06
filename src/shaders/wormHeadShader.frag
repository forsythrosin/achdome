#version 330 core

uniform sampler2D fboTex;

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
  // headColor.a *= 1.0 - step(0.9992, distToCenter);
  // headColor.a *= 1.0 - step(1.0, distToCenter);
  color = headColor;
}
