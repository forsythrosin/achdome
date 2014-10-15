#include <renderableWormGroup.h>
#include <glm/gtc/quaternion.hpp>
#include <cassert>
#include <iostream>

RenderableWormGroup::RenderableWormGroup(int wormCount, int segsPerWorm) {
  assert(segsPerWorm != 0);

  this->segsPerWorm = segsPerWorm;
  vertsPerWorm = segsPerWorm + 1; // e.g. 3 segs need 4 verts
  vertexCount = wormCount*vertsPerWorm;
  elementCount = wormCount*segsPerWorm;

  sphericalVertexData = new GLfloat[vertexCount*2];
  cartesianVertexData = new GLfloat[vertexCount*3];
  elementData = new GLuint[elementCount*VERTS_PER_ELEMENT];
};

RenderableWormGroup::~RenderableWormGroup() {};

void RenderableWormGroup::setWormArcs(std::vector<WormArc> wormArcs) {
  this->wormArcs = wormArcs;
  createVertices();
  createElements();
  update = true;
}

void RenderableWormGroup::createVertices() {
  for (int j = 0; j < wormArcs.size(); ++j) {
    WormArc arc = wormArcs.at(j);

    // each vertex
    for (int i = 0; i < vertsPerWorm; ++i) {
      int cartIdx = (j*vertsPerWorm + i)*3;
      int sphereIdx = (j*vertsPerWorm + i)*2;

      float t = (float)i/(float)segsPerWorm;
      // std::cout << "t = " << t << std::endl;

      glm::vec3 coord = arc.getCartesianLerp(t);
      // std::cout << "(" << coord.x << ", " << coord.y << ", " << coord.z << ")" << std::endl;
      cartesianVertexData[cartIdx] = coord.x;
      cartesianVertexData[cartIdx + 1] = coord.y;
      cartesianVertexData[cartIdx + 2] = coord.z;

      sphericalVertexData[sphereIdx] =
          coord.x != 0.0 ? atan(coord.y/coord.x) : glm::half_pi<float>();
      sphericalVertexData[sphereIdx + 1] = acos(coord.z/glm::length(coord));
    }
  }
};

/**
 * Elaborate way of indexing line segments.
 * But at least we understand it.
 */
void RenderableWormGroup::createElements() {
  for (int j = 0; j < wormArcs.size(); ++j) {
    for (int i = 0; i < segsPerWorm; ++i) {
      int elementIdx = (j*segsPerWorm + i)*VERTS_PER_ELEMENT;
      int vertexIdx = j*segsPerWorm + i;

      elementData[elementIdx] = vertexIdx;
      elementData[elementIdx + 1] = vertexIdx + 1;
    }
  }
};

const GLuint RenderableWormGroup::getVertsPerElement() const {
  return VERTS_PER_ELEMENT;
};

const std::vector<WormArc> RenderableWormGroup::getWormArcs() const {
  return wormArcs;
};
