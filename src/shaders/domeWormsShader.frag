#version 330 core

uniform sampler2D fboTex;
uniform vec2 fboTexSize;

out vec4 color;

void main() {
  float sampleX = (gl_FragCoord.s)/fboTexSize.x;
  float sampleY = (gl_FragCoord.t)/fboTexSize.y;
  vec2 sampleCoord = vec2(sampleX, sampleY);
  color = texture(fboTex, sampleCoord);
}
