#version 330 core

uniform sampler2D fboTex;

in vec4 vColor;
out vec4 color;

void main() {
  color = vColor;
}
