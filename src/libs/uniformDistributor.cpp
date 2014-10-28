#include <uniformDistributor.h>
#include <glm/gtx/constants.hpp>
#include <glm/glm.hpp>
#include <wormHead.h>
#include <iostream>
#include <glm/gtc/quaternion.hpp>

UniformDistributor::UniformDistributor() {
  std::random_device rd;
  randGen = std::mt19937(rd());
}

UniformDistributor::~UniformDistributor() {

}

bool UniformDistributor::distribute(std::map<int, WormHead*> wormHeads) {
  std::uniform_real_distribution<float> phiDis(0.0, 2.0 * glm::pi<float>());
  std::uniform_real_distribution<float> vDis(0.0, 1.0);
  std::uniform_real_distribution<float> aDis(0.0, 2 * glm::pi<float>());

  glm::vec3 quatOrigin(1.0, 0.0, 0.0);

  for (const auto &pair : wormHeads) {
    WormHead *wormHead = pair.second;
    // Position
    float v = vDis(randGen); // [0.1, 1.0[

    // Uniform distribution of spherical coordinates
    float phi = phiDis(randGen); // [0, 2*pi[
    float theta = glm::pi<float>()/2 - acosf(v);

    float x = sinf(theta) * cosf(phi);
    float y = sinf(theta) * sinf(phi);
    float z = cosf(theta);

    glm::vec3 position(x, y, z);

    glm::vec3 thetaAxis(0.0, 1.0, 0.0);
    float thetaAngle = theta - glm::pi<float>() / 2;
    glm::vec3 normThetaAxis = thetaAxis * sinf(thetaAngle / 2.0);
    glm::quat thetaQuat(cosf(thetaAngle / 2.0), normThetaAxis.x, normThetaAxis.y, normThetaAxis.z);

    glm::vec3 phiAxis(0.0, 0.0, 1.0);
    float phiAngle = phi;
    glm::vec3 normPhiAngle = phiAxis * sinf(phiAngle / 2.0);
    glm::quat phiQuat(cosf(phiAngle / 2.0), normPhiAngle.x, normPhiAngle.y, normPhiAngle.z);

    glm::quat posQuat = phiQuat * thetaQuat;

    glm::vec3 res = glm::mat3_cast(posQuat) * quatOrigin;
    wormHead->setPosition(posQuat);

    // Velocity is a quaternion with an axis perpendicular to the position vector
    // Get any vector perpendicular to the position vector
    glm::vec3 velAxis = glm::cross(quatOrigin, position);

    // Rotate the velocity axis about the position vector with an angle uniformly distributed between 0 and 2*pi
    float velAxisRotAngle = aDis(randGen); // [0, 2*pi[
    glm::vec3 velAxisRotAxis = position;

    // Normalize axis for quaterion
    glm::vec3 normVelAxisRotAxis = velAxisRotAxis * sinf(velAxisRotAngle / 2.0);

    // Create quaterion to rotate the rotation axis
    glm::quat velAxisRotationQuat(cosf(velAxisRotAngle / 2.0), normVelAxisRotAxis.x, normVelAxisRotAxis.y, normVelAxisRotAxis.z);

    // Random axis perpendicular to the position vector
    velAxis = velAxis * velAxisRotationQuat;

    // Normalize velocity axis for quaterion
    glm::vec3 normVelAxis = velAxis * sinf(velocityAngle / 2.0);

    // Create velocity quaternion
    glm::quat velQuat(cosf(velocityAngle / 2.0), normVelAxis.x, normVelAxis.y, normVelAxis.z);
    wormHead->setVelocity(velQuat);

    glm::vec3 velRotAxis = glm::axis(velQuat);
  }
  return true;
}