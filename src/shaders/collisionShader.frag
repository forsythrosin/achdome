#version 330 core

uniform vec4[100] collisions;
uniform vec4[100] collisionColors;
uniform uint collisionCount;

in vec3 cartesianPos;

out vec4 color;

void main() {
  vec4 shockWaveColor = vec4(0.0, 0.0, 0.0, 0.0);
  float clampRadius = 0.5;
  float thickness = 0.025;
  uint i;

  // for each collision
  for (i = 0u ; i < collisionCount; ++i) {
    vec3 epicentrum = collisions[i].xyz;
    float life = collisions[i].a;
    float shockwaveRadius = (1.0 - life)*2.0;

    float distFromEpicentrum = distance(cartesianPos, epicentrum);

    float localShockwave = smoothstep(shockwaveRadius - thickness, shockwaveRadius + thickness, distFromEpicentrum);
    localShockwave *= (1.0 - smoothstep(shockwaveRadius - thickness, shockwaveRadius + thickness, distFromEpicentrum));
    localShockwave *= ((clampRadius - thickness)/distFromEpicentrum);

    vec4 localShockwaveColor = vec4(collisionColors[i].xyz, localShockwave);
    shockWaveColor += localShockwave > 0 ? localShockwaveColor : vec4(0.0, 0.0, 0.0, 0.0);
  }

  color = shockWaveColor;
}
