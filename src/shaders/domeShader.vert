#version 330 core

layout(location = 0) in vec2 phiTheta;
uniform mat4 MVP;

out vec2 phiThetaCoords;

void main() {
  // static dome radius: 7.5m
  float r = 7.5;

  // Output position of the vertex, in clip space : MVP * position
  vec3 vertPosition = vec3(
    r*sin(phiTheta.y)*cos(phiTheta.x),
    r*sin(phiTheta.y)*sin(phiTheta.x),
    r*cos(phiTheta.y)
  );

  phiThetaCoords = phiTheta;
  gl_Position =  MVP * vec4(vertPosition, 1.0);
}
