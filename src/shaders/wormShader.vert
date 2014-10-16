#version 330 core

layout(location = 0) in vec3 pos;
uniform mat4 MVP;

void main() {
  // static dome radius: 7.5m
  float r = 7.5;
  gl_Position =  MVP * vec4(pos*r, 1.0);
}
