#include <uniformDistributor.h>
#include <glm/gtx/constants.hpp>
#include <glm/glm.hpp>
#include <wormHead.h>
#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include <gameConfig.h>

UniformDistributor::UniformDistributor(GameConfig *gameConfig) : WormHeadDistributor(gameConfig) {
  std::random_device rd;
  randGen = std::mt19937(rd());
}

UniformDistributor::~UniformDistributor() {

}

bool UniformDistributor::distribute(std::map<int, WormHead*> wormHeads) {
  std::uniform_real_distribution<float> phiDis(0.0, 2.0 * glm::pi<float>());
  std::uniform_real_distribution<float> vDis(0.1, 1.0);
  std::uniform_real_distribution<float> aDis(0.0, 2.0 * glm::pi<float>());

  float velocityAngle = gameConfig->wormSpeed;

  glm::dvec3 quatOrigin(1.0, 0.0, 0.0);

  for (const auto &pair : wormHeads) {
    WormHead *wormHead = pair.second;
    // Position
    double v = vDis(randGen); // [0.1, 1.0[

    // Uniform distribution of spherical coordinates
    double phi = phiDis(randGen); // [0, 2*pi[
    double theta = acosf(v);

    double x = sin(theta) * cos(phi);
    double y = sin(theta) * sin(phi);
    double z = cos(theta);

    glm::dvec3 position(x, y, z);

    glm::dvec3 thetaAxis(0.0, 1.0, 0.0);
    double thetaAngle = theta - glm::pi<double>() / 2;
    glm::dvec3 normThetaAxis = thetaAxis * sin(thetaAngle / 2.0);
    glm::dquat thetaQuat(cosf(thetaAngle / 2.0), normThetaAxis.x, normThetaAxis.y, normThetaAxis.z);

    glm::dvec3 phiAxis(0.0, 0.0, 1.0);
    double phiAngle = phi;
    glm::dvec3 normPhiAngle = phiAxis * sin(phiAngle / 2.0);
    glm::dquat phiQuat(cosf(phiAngle / 2.0), normPhiAngle.x, normPhiAngle.y, normPhiAngle.z);

    glm::dquat posQuat = phiQuat * thetaQuat;

    glm::dvec3 res = glm::mat3_cast(posQuat) * quatOrigin;
    wormHead->setPosition(posQuat);

    // Velocity is a quaternion with an axis perpendicular to the position vector
    // Get any vector perpendicular to the position vector
    glm::dvec3 velAxis = glm::cross(quatOrigin, position);

    // Rotate the velocity axis about the position vector with an angle uniformly distributed between 0 and 2*pi
    double velAxisRotAngle = aDis(randGen); // [0, 2*pi[
    glm::dvec3 velAxisRotAxis = position;

    // Normalize axis for quaterion
    glm::dvec3 normVelAxisRotAxis = velAxisRotAxis * sin(velAxisRotAngle / 2.0);

    // Create quaterion to rotate the rotation axis
    glm::dquat velAxisRotationQuat(cos(velAxisRotAngle / 2.0), normVelAxisRotAxis.x, normVelAxisRotAxis.y, normVelAxisRotAxis.z);

    // Random axis perpendicular to the position vector
    velAxis = velAxis * velAxisRotationQuat;

    // Normalize velocity axis for quaterion
    glm::dvec3 normVelAxis = velAxis * sin(velocityAngle / 2.0);

    // Create velocity quaternion
    glm::dquat velQuat(cosf(velocityAngle / 2.0), normVelAxis.x, normVelAxis.y, normVelAxis.z);
    wormHead->setVelocity(velQuat);

    glm::dvec3 velRotAxis = glm::axis(velQuat);
  }
  return true;
}
