#include <renderableWormHeads.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext.hpp>
#include <cassert>
#include <iostream>

/*
uniform float strokeWidth;
uniform float arrowWidth;
uniform float arrowLength;
*/

RenderableWormHeads::RenderableWormHeads(int wormCount, GLfloat wormThickness) {
  this->wormThickness = wormThickness;
  vertexCount = wormCount*VERTS_PER_HEAD;
  elementCount = wormCount*ELEMENTS_PER_HEAD;

  sphericalVertexData.reserve(vertexCount*2);
  cartesianVertexData.reserve(vertexCount*3);
  vertexColorData.reserve(vertexCount*4);
  elementData.reserve(elementCount*VERTS_PER_ELEMENT);

  headCenterData.reserve(vertexCount*3);
  headDirectionData.reserve(vertexCount*3);

  headCenterBuffer = -1;
  headDirectionBuffer = -1;
};

RenderableWormHeads::~RenderableWormHeads() {};

void RenderableWormHeads::setWormHeads(std::vector<WormHeadSyncData> wormHeads) {
  this->wormHeads = wormHeads;
  std::vector<glm::vec4> colors;
  for (WormHeadSyncData wh : wormHeads) {
    colors.push_back(wh.getColor());
  }
  setWormColors(colors);
  createVertices();
  createElements();
  update = true;
};

void RenderableWormHeads::setWormColors(std::vector<glm::vec4> wormColors) {
  this->wormColors = wormColors;
  vertexColorData.clear();
  for (int j = 0; j < wormColors.size(); ++j) {
    glm::vec4 color = wormColors.at(j);

    for (int i = 0; i < VERTS_PER_HEAD; ++i) {
      vertexColorData.push_back(color[0]);
      vertexColorData.push_back(color[1]);
      vertexColorData.push_back(color[2]);
      vertexColorData.push_back(color[3]);
    }
  }

  update = true;
};

void RenderableWormHeads::createVertices() {
  cartesianVertexData.clear();
  headCenterData.clear();
  headDirectionData.clear();
  appearanceData.clear();

  for (int j = 0; j < wormHeads.size(); ++j) {
    WormHeadSyncData wh = wormHeads.at(j);
    glm::vec3 headPos = (glm::vec3) wh.getPosition();
    glm::vec3 headDirection = glm::normalize((glm::vec3) wh.getVelocity());
    glm::vec3 domeNormal = glm::normalize(headPos);

    for (int i = 0; i < VERTS_PER_HEAD; ++i) {
      glm::vec3 quadPoint = headPos + glm::rotate(headDirection, i*360.0f/VERTS_PER_HEAD, domeNormal);

      cartesianVertexData.push_back(quadPoint.x);
      cartesianVertexData.push_back(quadPoint.y);
      cartesianVertexData.push_back(quadPoint.z);

      headCenterData.push_back(headPos.x);
      headCenterData.push_back(headPos.y);
      headCenterData.push_back(headPos.z);

      headDirectionData.push_back(headDirection.x);
      headDirectionData.push_back(headDirection.y);
      headDirectionData.push_back(headDirection.z);

      float strokeWidth = wh.getWidth();
      float arrowWidth = wh.getArrowWidth();
      float arrowLength = wh.getArrowLength();

      appearanceData.push_back(strokeWidth);
      appearanceData.push_back(arrowWidth);
      appearanceData.push_back(arrowLength);
    }
  }
};

void RenderableWormHeads::createElements() {
  elementData.clear();
  for (int i = 0; i < wormHeads.size(); ++i) {
    int first = i*VERTS_PER_HEAD;
    int second = i*VERTS_PER_HEAD + 1;
    int third = i*VERTS_PER_HEAD + 2;
    int fourth = i*VERTS_PER_HEAD + 3;

    elementData.push_back(first);
    elementData.push_back(second);
    elementData.push_back(third);

    elementData.push_back(first);
    elementData.push_back(third);
    elementData.push_back(fourth);
  }
};

/**
 * Overridden to upload attributes specific to wormHeads
 */
void RenderableWormHeads::loadToGPU(bool sphericalCoords) {
  Renderable::loadToGPU(sphericalCoords);
  glBindVertexArray(vertexArray);

  GLuint vertexDim = 3;

  glBindBuffer(GL_ARRAY_BUFFER, headCenterBuffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    headCenterData.size()*sizeof(GLfloat),
    headCenterData.data(),
    GL_STATIC_DRAW
  );
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, vertexDim, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, headDirectionBuffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    headDirectionData.size()*sizeof(GLfloat),
    headDirectionData.data(),
    GL_STATIC_DRAW
  );
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, vertexDim, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, appearanceBuffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    appearanceData.size()*sizeof(GLfloat),
    appearanceData.data(),
    GL_STATIC_DRAW
  );
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, vertexDim, GL_FLOAT, GL_FALSE, 0, 0);

  // Unbind
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);
  glDisableVertexAttribArray(4);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderableWormHeads::attach() {
  Renderable::attach();
  glGenBuffers(1, &headCenterBuffer);
  glGenBuffers(1, &headDirectionBuffer);
  glGenBuffers(1, &appearanceBuffer);
}

void RenderableWormHeads::detach() {
  glDeleteBuffers(1, &headDirectionBuffer);
  glDeleteBuffers(1, &headCenterBuffer);
  glDeleteBuffers(1, &appearanceBuffer);
  Renderable::detach();
}

void RenderableWormHeads::enableAttributes() {
  Renderable::enableAttributes();
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(4);
}

void RenderableWormHeads::disableAttributes() {
  glDisableVertexAttribArray(4);
  glDisableVertexAttribArray(3);
  glDisableVertexAttribArray(2);
  Renderable::disableAttributes();
}

const GLuint RenderableWormHeads::getVertsPerElement() const {
  return VERTS_PER_ELEMENT;
};

const std::vector<WormHeadSyncData> RenderableWormHeads::getWormHeads() const {
  return wormHeads;
};
