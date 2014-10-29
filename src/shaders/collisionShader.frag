#version 330 core

uniform float time;
uniform vec3[100] collisionPos;

out vec4 color;

void main() {
  color = vec4(1.0, 1.0, 1.0, 0.5)*sin(time);
}
