#version 330 core

uniform sampler2D text;

out vec4 outColor;

in vec4 vColor;
in vec2 texCoords;

void main() {
  outColor = texture(text, texCoords.xy);
}
