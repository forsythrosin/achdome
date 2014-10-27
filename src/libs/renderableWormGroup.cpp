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
  vertexColorData = new GLfloat[vertexCount*4];
  elementData = new GLuint[elementCount*VERTS_PER_ELEMENT];
};

RenderableWormGroup::~RenderableWormGroup() {};

void RenderableWormGroup::setWormArcs(std::vector<WormArc> wormArcs) {
  this->wormArcs = wormArcs;
  createVertices();
  createElements();
  update = true;
};

void RenderableWormGroup::setWormColors(std::vector<glm::vec3> wormColors) {
  std::vector<glm::vec4> colors(wormColors.size());

  for (int i = 0; i < wormColors.size(); ++i) {
    colors.push_back(glm::vec4(wormColors.at(i), 0.0));
  }

  setWormColors(colors);
};

void RenderableWormGroup::setWormColors(std::vector<glm::vec4> wormColors) {
  this->wormColors = wormColors;

  for (int j = 0; j < wormColors.size(); ++j) {
    glm::vec4 color = wormColors.at(j);

    for (int i = 0; i < vertsPerWorm; ++i) {
      int r = (j*vertsPerWorm + i)*4;
      int g = (j*vertsPerWorm + i)*4 + 1;
      int b = (j*vertsPerWorm + i)*4 + 2;
      int a = (j*vertsPerWorm + i)*4 + 3;

      vertexColorData[r] = color[0];
      vertexColorData[g] = color[1];
      vertexColorData[b] = color[2];
      vertexColorData[a] = color[3];
    }
  }

  update = true;
};

void RenderableWormGroup::createVertices() {
  for (int j = 0; j < wormArcs.size(); ++j) {
    WormArc arc = wormArcs.at(j);

    // each vertex
    for (int i = 0; i < vertsPerWorm; ++i) {
      int cartIdx = (j*vertsPerWorm + i)*3;
      int sphereIdx = (j*vertsPerWorm + i)*2;

      float t = (float)i/(float)segsPerWorm;

      glm::vec3 coord = arc.getCartesianLerp(t);
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
      int vertexIdx = j*vertsPerWorm+ i;

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
