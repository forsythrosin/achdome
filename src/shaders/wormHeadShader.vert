#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec3 headCenterPos;
layout(location = 3) in vec3 headDirectionAttr;
layout(location = 4) in vec4 appearanceAttr;
uniform mat4 MVP;

out vec4 vColor;
out vec3 center;
out vec3 worldPos;
out vec3 headDirection;
out vec4 appearance;

void main() {
  vColor = vertexColor;
  center = headCenterPos;
  worldPos = pos;
  headDirection = headDirectionAttr;
  appearance = appearanceAttr;

  // static dome radius: 7.5m
  float r = 7.5;
  gl_Position = MVP * vec4(pos*r, 1.0);
}
