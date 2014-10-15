#version 330 core

in vec2 phiThetaCoords;
out vec4 color;

void main() {
  const float PI = 3.1415;
  color = vec4(phiThetaCoords.x/(PI*2.0), phiThetaCoords.y/(PI/2.0), 1.0, 0.8);
}
