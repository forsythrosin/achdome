#version 330 core

uniform sampler2D fboTex;
uniform vec2 fboTexSize;

out vec4 color;

void main() {
  color = vec4(0.0);

  // Blur
  // float offsetX, offsetY;
  // for (offsetY = -5.0; offsetY <= 5.0; offsetY += 1.0) {
  //   for (offsetY = -5.0; offsetY <= 5.0; offsetY += 1.0) {
  //     float sampleX = (gl_FragCoord.s + offsetX)/fboTexSize.x;
  //     float sampleY = (gl_FragCoord.t + offsetY)/fboTexSize.y;
  //     vec2 sampleCoord = vec2(sampleX,sampleY);
  //     color.rgb += texture(fboTex, sampleCoord).rgb;
  //   }
  // }
  // color.a = 1.0;

  float sampleX = (gl_FragCoord.s)/fboTexSize.x;
  float sampleY = (gl_FragCoord.t)/fboTexSize.y;
  vec2 sampleCoord = vec2(sampleX, sampleY);
  color = texture(fboTex, sampleCoord);
}
