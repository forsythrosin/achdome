#version 330 core

uniform sampler2DMS fboTex;
uniform vec2 fboTexSize;

out vec4 color;

void main() {
  float sampleX = (gl_FragCoord.s)/fboTexSize.x;
  float sampleY = (gl_FragCoord.t)/fboTexSize.y;
  int fxaaSamples = 8;
  vec2 sampleCoord = vec2(sampleX, sampleY);

  color = vec4(0);
  for(int i = 0; i < fxaaSamples; ++i) {
    color += texelFetch(fboTex, ivec2(gl_FragCoord), i);
  }

  color /= fxaaSamples;
}
