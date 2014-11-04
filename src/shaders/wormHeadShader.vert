#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec3 headCenterPos;
uniform mat4 MVP;

out vec4 vColor;
out vec3 center;
out vec3 worldPos;

void main() {
  vColor = vertexColor;
  center = headCenterPos;
  worldPos = pos;

  // static dome radius: 7.5m
  float r = 7.5;
  gl_Position = MVP * vec4(pos*r, 1.0);
}
