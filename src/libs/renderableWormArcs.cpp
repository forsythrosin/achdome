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

  sphericalVertexData = new GLfloat[vertexCount*2];
  cartesianVertexData = new GLfloat[vertexCount*3];
  vertexColorData = new GLfloat[vertexCount*4];
  elementData = new GLuint[elementCount*VERTS_PER_ELEMENT];
};

RenderableWormArcs::~RenderableWormArcs() {};

void RenderableWormArcs::setWormArcs(std::vector<WormArc> wormArcs) {
  this->wormArcs = wormArcs;
  std::vector<glm::vec4> colors;
  for (WormArc wa : wormArcs) {
    colors.push_back(wa.getColor());
  }
  setWormColors(colors);
  createVertices();
  createElements();
  update = true;
};

void RenderableWormArcs::setWormColors(std::vector<glm::vec4> wormColors) {
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

void RenderableWormArcs::createVertices() {
  float refVertsPerWorm = segsPerWorm + 1;
  for (int j = 0; j < wormArcs.size(); ++j) {
    WormArc arc = wormArcs.at(j);

    // each vertex
    for (int i = 0; i < refVertsPerWorm; ++i) {
      int cartIdx0 = (j*refVertsPerWorm + i)*6;
      int cartIdx1 = (j*refVertsPerWorm + i)*6 + 3;

      int sphereIdx0 = (j*refVertsPerWorm + i)*4;
      int sphereIdx1 = (j*refVertsPerWorm + i)*4 + 2;

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

      cartesianVertexData[cartIdx0] = posSpread0.x;
      cartesianVertexData[cartIdx0 + 1] = posSpread0.y;
      cartesianVertexData[cartIdx0 + 2] = posSpread0.z;

      cartesianVertexData[cartIdx1] = posSpread1.x;
      cartesianVertexData[cartIdx1 + 1] = posSpread1.y;
      cartesianVertexData[cartIdx1 + 2] = posSpread1.z;

      sphericalVertexData[sphereIdx0] =
        posSpread0.x != 0.0 ? atan(posSpread0.y / posSpread0.x) : glm::half_pi<double>();
      sphericalVertexData[sphereIdx0 + 1] = acos(posSpread0.z/glm::length(posSpread0));
      sphericalVertexData[sphereIdx1] =
        posSpread1.x != 0.0 ? atan(posSpread1.y / posSpread1.x) : glm::half_pi<double>();
      sphericalVertexData[sphereIdx1 + 1] = acos(posSpread1.z/glm::length(posSpread1));
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
  for (int j = 0; j < wormArcs.size(); ++j) {
    for (int i = 0; i < segsPerWorm; ++i) {
      int elementIdx0 = (j*segsPerWorm + i)*VERTS_PER_ELEMENT*2;
      int elementIdx1 = (j*segsPerWorm + i)*VERTS_PER_ELEMENT*2 + VERTS_PER_ELEMENT;

      // vertices
      int currentLeft = j*vertsPerWorm + i*2;
      int currentRight = j*vertsPerWorm + i*2 + 1;
      int nextLeft = j*vertsPerWorm + i*2 + 2;
      int nextRight = j*vertsPerWorm + i*2 + 3;

      elementData[elementIdx0] = currentLeft;
      elementData[elementIdx0 + 1] = currentRight;
      elementData[elementIdx0 + 2] = nextRight;

      elementData[elementIdx1] = currentLeft;
      elementData[elementIdx1 + 1] = nextRight;
      elementData[elementIdx1 + 2] = nextLeft;
    }
  }
};

const GLuint RenderableWormArcs::getVertsPerElement() const {
  return VERTS_PER_ELEMENT;
};

const std::vector<WormArc> RenderableWormArcs::getWormArcs() const {
  return wormArcs;
};
