#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec3 arcFrontPos;
layout(location = 3) in vec3 arcBackPos;
layout(location = 4) in float arcWidthAttr;
layout(location = 5) in float playerId;
layout(location = 6) in float arcTime;
uniform mat4 MVP;

out vec4 vColor;
out vec3 worldPos;
out vec3 arcFront;
out vec3 arcBack;
out float arcWidth;
out float pId;
out float vTime;

void main() {
  worldPos = pos;
  arcFront = arcFrontPos;
  arcBack = arcBackPos;
  arcWidth = arcWidthAttr;
  pId = playerId;
  vTime = arcTime;

  // static dome radius: 7.5m
  float r = 7.5;
  vColor = vertexColor;
  gl_Position =  MVP * vec4(pos*r, 1.0);
}
