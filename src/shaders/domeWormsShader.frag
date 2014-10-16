#version 330 core

uniform sampler2D fboTex;
uniform vec2 fboTexSize;

out vec4 color;

void main() {
  // paint wormLine fbo as texture
  color = texture(fboTex, vec2(gl_FragCoord.s/fboTexSize.x, gl_FragCoord.t/fboTexSize.y));
}
