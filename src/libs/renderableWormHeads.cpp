#include <renderableWormHeads.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext.hpp>
#include <cassert>
#include <iostream>

RenderableWormHeads::RenderableWormHeads(int wormCount, GLfloat wormThickness) {
  this->wormThickness = wormThickness;
  vertexCount = wormCount*VERTS_PER_HEAD;
  elementCount = wormCount*ELEMENTS_PER_HEAD;

  sphericalVertexData = new GLfloat[vertexCount*2];
  cartesianVertexData = new GLfloat[vertexCount*3];
  vertexColorData = new GLfloat[vertexCount*4];
  elementData = new GLuint[elementCount*VERTS_PER_ELEMENT];
};

RenderableWormHeads::~RenderableWormHeads() {};

void RenderableWormHeads::setWormHeads(std::vector<WormHead> wormHeads) {
  this->wormHeads = wormHeads;
  std::vector<glm::vec4> colors;
  for (WormHead wh : wormHeads) {
    colors.push_back(wh.getColor());
  }
  setWormColors(colors);
  createVertices();
  createElements();
  update = true;
};

void RenderableWormHeads::setWormColors(std::vector<glm::vec4> wormColors) {
  this->wormColors = wormColors;

  for (int j = 0; j < wormColors.size(); ++j) {
    glm::vec4 color = wormColors.at(j);

    for (int i = 0; i < VERTS_PER_HEAD; ++i) {
      int r = (j*VERTS_PER_HEAD + i)*4;
      int g = (j*VERTS_PER_HEAD + i)*4 + 1;
      int b = (j*VERTS_PER_HEAD + i)*4 + 2;
      int a = (j*VERTS_PER_HEAD + i)*4 + 3;

      vertexColorData[r] = color[0];
      vertexColorData[g] = color[1];
      vertexColorData[b] = color[2];
      vertexColorData[a] = color[3];
    }
  }

  update = true;
};

void RenderableWormHeads::createVertices() {
  for (int j = 0; j < wormHeads.size(); ++j) {
    WormHead wh = wormHeads.at(j);
    glm::vec3 headPos = (glm::vec3) wh.getPosition();
    glm::vec3 headDirection = glm::normalize((glm::vec3) wh.getVelocity())*wormThickness;
    glm::vec3 domeNormal = headPos; // for convenience, already normalized

    for (int i = 0; i < VERTS_PER_HEAD; ++i) {
      int cartIdx = (j*VERTS_PER_HEAD + i)*3;
      int sphereIdx = (j*VERTS_PER_HEAD + i)*2;

      glm::vec3 quadPoint = headPos + glm::rotate(headDirection, i*360.0f/VERTS_PER_HEAD, domeNormal);

      cartesianVertexData[cartIdx] = quadPoint.x;
      cartesianVertexData[cartIdx + 1] = quadPoint.y;
      cartesianVertexData[cartIdx + 2] = quadPoint.z;

      // sphericalVertexData[cartIdx] = headPos.x;
      // sphericalVertexData[cartIdx + 1] = headPos.y;
    }
  }
};

void RenderableWormHeads::createElements() {
  for (int i = 0; i < wormHeads.size(); ++i) {
    int elementIdx0 = i*ELEMENTS_PER_HEAD*VERTS_PER_ELEMENT;
    int elementIdx1 = i*ELEMENTS_PER_HEAD*VERTS_PER_ELEMENT + VERTS_PER_ELEMENT;

    int first = i*VERTS_PER_HEAD;
    int second = i*VERTS_PER_HEAD + 1;
    int third = i*VERTS_PER_HEAD + 2;
    int fourth = i*VERTS_PER_HEAD + 3;

    elementData[elementIdx0] = first;
    elementData[elementIdx0 + 1] = second;
    elementData[elementIdx0 + 2] = third;

    elementData[elementIdx1] = first;
    elementData[elementIdx1 + 1] = third;
    elementData[elementIdx1 + 2] = fourth;
  }
};

const GLuint RenderableWormHeads::getVertsPerElement() const {
  return VERTS_PER_ELEMENT;
};

const std::vector<WormHead> RenderableWormHeads::getWormHeads() const {
  return wormHeads;
};
