#include <renderableWormArcs.h>
#include <glm/gtc/quaternion.hpp>
#include <cassert>
#include <iostream>

RenderableWormArcs::RenderableWormArcs(int wormCount, int segsPerWorm) {
  assert(segsPerWorm != 0);

  this->segsPerWorm = segsPerWorm;
  vertsPerWorm = (segsPerWorm + 1)*2; // e.g. 3 segs need 8 vertices -> 6 triangles
  vertexCount = wormCount*vertsPerWorm;
  elementCount = wormCount*segsPerWorm*2; // 2 triangles per segment

  sphericalVertexData.reserve(vertexCount*2);
  cartesianVertexData.reserve(vertexCount*3);
  vertexColorData.reserve(vertexCount*4);
  elementData.reserve(elementCount*VERTS_PER_ELEMENT);
  
};

RenderableWormArcs::~RenderableWormArcs() {};

void RenderableWormArcs::setWormArcs(std::vector<WormArcSyncData> wormArcs) {
  this->wormArcs = wormArcs;
  std::vector<glm::vec4> colors;
  for (WormArcSyncData wa : wormArcs) {
    colors.push_back(wa.getColor());
  }
  setWormColors(colors);
  createVertices();
  createElements();
  update = true;
};

void RenderableWormArcs::setWormColors(std::vector<glm::vec4> wormColors) {
  this->wormColors = wormColors;
  vertexColorData.clear();
  for (int j = 0; j < wormColors.size(); ++j) {
    glm::vec4 color = wormColors.at(j);

    for (int i = 0; i < vertsPerWorm; ++i) {
      vertexColorData.push_back(color[0]);
      vertexColorData.push_back(color[1]);
      vertexColorData.push_back(color[2]);
      vertexColorData.push_back(color[3]);
    }
  }

  update = true;
};

void RenderableWormArcs::createVertices() {
  float refVertsPerWorm = segsPerWorm + 1;
  cartesianVertexData.clear();
  sphericalVertexData.clear();
  for (int j = 0; j < wormArcs.size(); ++j) {
    WormArcSyncData arc = wormArcs.at(j);

    // each vertex
    for (int i = 0; i < refVertsPerWorm; ++i) {

      double t = (double)i/(double)segsPerWorm;
      glm::dvec3 pos = arc.getCartesianLerp(t);

      // Calculate "spread" vectors on the dome surface
      int iNext = (i + 1);
      bool isLast = iNext >= refVertsPerWorm;
      // If last ref vert, take prev vert. Else next vert
      iNext = isLast ? i - 1 : iNext;
      double tNext = (double)iNext / (double)segsPerWorm;
      // If last ref vert, flip vector sign
      glm::dvec3 toNext = isLast ?
          -arc.getCartesianLerp(tNext) : arc.getCartesianLerp(tNext);

      glm::dvec3 dSpreadVec = glm::normalize(glm::cross(pos, toNext));
      glm::vec3 spreadVec(dSpreadVec);
      glm::vec3 posSpread0 = glm::vec3(pos) + spreadVec*arc.getWidth()*0.5f;
      glm::vec3 posSpread1 = glm::vec3(pos) - spreadVec*arc.getWidth()*0.5f;
      
      cartesianVertexData.push_back(posSpread0.x);
      cartesianVertexData.push_back(posSpread0.y);
      cartesianVertexData.push_back(posSpread0.z);

      cartesianVertexData.push_back(posSpread1.x);
      cartesianVertexData.push_back(posSpread1.y);
      cartesianVertexData.push_back(posSpread1.z);

      sphericalVertexData.push_back(posSpread0.x != 0.0 ? atan(posSpread0.y / posSpread0.x) : glm::half_pi<double>());
      sphericalVertexData.push_back(acos(posSpread0.z/glm::length(posSpread0)));
      sphericalVertexData.push_back(posSpread1.x != 0.0 ? atan(posSpread1.y / posSpread1.x) : glm::half_pi<double>());
      sphericalVertexData.push_back(acos(posSpread1.z/glm::length(posSpread1)));
    }
  }
};

/**
 * Create triangles for each worm segment. Elements are evaluated as follows:
 *
 * currentLeft         nextLeft         ...
 *      |                 |              |
 * currentRef -------- nextRef -------- ...
 *      |                 |              |
 * currentRight        nextRight        ...
 *
 */
void RenderableWormArcs::createElements() {
  elementData.clear();
  for (int j = 0; j < wormArcs.size(); ++j) {
    for (int i = 0; i < segsPerWorm; ++i) {
      // vertices
      int currentLeft = j*vertsPerWorm + i*2;
      int currentRight = j*vertsPerWorm + i*2 + 1;
      int nextLeft = j*vertsPerWorm + i*2 + 2;
      int nextRight = j*vertsPerWorm + i*2 + 3;

      elementData.push_back(currentLeft);
      elementData.push_back(currentRight);
      elementData.push_back(nextRight);

      elementData.push_back(currentLeft);
      elementData.push_back(nextRight);
      elementData.push_back(nextLeft);
    }
  }
};

const GLuint RenderableWormArcs::getVertsPerElement() const {
  return VERTS_PER_ELEMENT;
};

const std::vector<WormArcSyncData> RenderableWormArcs::getWormArcs() const {
  return wormArcs;
};
