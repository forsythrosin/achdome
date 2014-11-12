#version 330 core

uniform float time;

in vec4 vColor;
in vec3 center;
in vec3 worldPos;
in vec3 headDirection;
in vec4 appearance;

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
  return distance(p0,p1) > 0.00001 ? dist : distToP0;
}

float aastep(float threshold, float value) {
  float afwidth = 0.7 * length(vec2(dFdx(value), dFdy(value)));
  return smoothstep(threshold-afwidth, threshold+afwidth, value);
}

void main() {
  float headDiameter = appearance.x;
  float strokeWidth = appearance.y;
  float arrowWidth = appearance.z;
  float arrowLength = appearance.w;

  vec4 headColor = vColor;
  vec3 perpDirection = cross(center, headDirection);

  vec3 p0 = center;
  vec3 p1 = center + headDirection * arrowLength;
  vec3 p2 = center + headDirection * arrowLength * 0.667 - perpDirection * arrowWidth * 0.5;
  vec3 p3 = center + headDirection * arrowLength * 0.667 + perpDirection * arrowWidth * 0.5;

  float minDist = min(min(distanceToLineSegment(worldPos, p0, p1),
                          distanceToLineSegment(worldPos, p1, p2)),
                          distanceToLineSegment(worldPos, p1, p3));

  float insideArrow = 1.0 - aastep(strokeWidth, minDist);
  float insideHead = 1.0 - aastep(headDiameter, distance(worldPos, p0));
  headColor.a = max(insideArrow, insideHead);

  color = headColor;
}
