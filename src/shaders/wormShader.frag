#version 330 core

in vec4 vColor;
in vec3 worldPos;
in vec3 arcFront;
in vec3 arcBack;
in float arcWidth;

out vec4 color;

float distanceToLineSegment(vec3 p, vec3 p0, vec3 p1) {
  vec3 v = normalize(p1 - p0);

  float t0 = dot(p0, v);
  float t1 = dot(p1, v);
  float t = dot(p, v);

  vec3 projected = p0 + (t - t0)*v;
  float distToLine = length(projected - p);

  float distToP0 = distance(p, p0);
  float distToP1 = distance(p, p1);
  float insideInterval = step(t0, t) * step(t, t1);
  float dist = mix(min(distToP0, distToP1), distToLine, insideInterval);

  return mix(distToP0, dist, step(0.00001, distance(p0, p1)));
}

float aastep(float threshold, float value) {
  float afwidth = 0.7 * length(vec2(dFdx(value), dFdy(value)));
  return smoothstep(threshold - afwidth, threshold+afwidth, value);
}

void main() {
  vec4 arcColor = vColor;
  float distToArc = distanceToLineSegment(worldPos, arcFront, arcBack);
  float insideArc = 1.0 - aastep(arcWidth, distToArc);
  arcColor.a = insideArc;

  color = arcColor;
}
