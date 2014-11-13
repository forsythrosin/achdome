#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec2 textureCoords;
uniform mat4 MVP;

out vec4 vColor;
out vec2 texCoords;

void main() {
  vColor = vertexColor;
  gl_Position = MVP * vec4(pos, 1.0);
  texCoords = textureCoords;
}
