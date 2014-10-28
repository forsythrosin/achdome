#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 vertexColor;
uniform mat4 MVP;

out vec4 vColor;

void main() {
  vColor = vertexColor;
  gl_Position =  MVP * vec4(pos, 1.0);
}
