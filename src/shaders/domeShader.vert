#version 330 core

layout(location = 0) in vec2 phiTheta;
uniform mat4 MVP;
out vec2 sphericalPos;
out vec3 cartesianPos;

void main() {
  // static dome radius: 7.5m
  float r = 7.5;

  // spherical -> cartesian
  vec3 vertPosition = vec3(
    sin(phiTheta.y)*cos(phiTheta.x),
    sin(phiTheta.y)*sin(phiTheta.x),
    cos(phiTheta.y)
  );

  sphericalPos = phiTheta;
  cartesianPos = vertPosition;

  gl_Position = MVP*vec4(vertPosition*r, 1.0);
}
