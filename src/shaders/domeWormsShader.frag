#version 330 core

uniform sampler2D fboTex;
uniform vec2 fboTexSize;

out vec4 color;

void main() {
  float sampleX = (gl_FragCoord.s)/fboTexSize.x;
  float sampleY = (gl_FragCoord.t)/fboTexSize.y;
  vec2 sampleCoord = vec2(sampleX, sampleY);
  vec4 sampleColor = texture(fboTex, sampleCoord);
  if (sampleColor.x < 0.9) {
    sampleColor.a = 0.1;
  }
  color = sampleColor;
}
