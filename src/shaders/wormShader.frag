#version 330 core

uniform sampler2D fboTex;

out vec4 color;

void main() {
  color = vec4(1.0, 0.0, 0.0, 1.0);
}
