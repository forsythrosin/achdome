#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 vertexColor;
uniform mat4 MVP;

out vec4 vColor;

void main() {
  // static dome radius: 7.5m
  float r = 7.5;
  vColor = vertexColor;
  gl_Position =  MVP * vec4(pos*r, 1.0);
}
