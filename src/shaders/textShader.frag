#version 330 core

uniform sampler2D text;

out vec4 color;

in vec4 vColor;
in vec2 texCoords;

void main() {
  vec4 sampleColor = texture(text, texCoords.xy);
  color = sampleColor*vColor;
}
