#include <renderableWormArcs.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext.hpp>
#include <cassert>
#include <iostream>

RenderableWormArcs::RenderableWormArcs(int wormCount, int segsPerWorm) {
  vertexCount = wormCount*VERTS_PER_ARC;
  elementCount = wormCount*ELEMENTS_PER_ARC;

  sphericalVertexData.reserve(vertexCount*2);
  cartesianVertexData.reserve(vertexCount*3);
  vertexColorData.reserve(vertexCount*4);
  elementData.reserve(elementCount*VERTS_PER_ELEMENT);

  arcFrontData.reserve(vertexCount*3);
  arcBackData.reserve(vertexCount*3);
  arcWidthData.reserve(vertexCount);

  arcFrontBuffer = -1;
  arcBackBuffer = -1;
  arcWidthBuffer = -1;
};

RenderableWormArcs::~RenderableWormArcs() {};

void RenderableWormArcs::setWormArcs(std::vector<WormArcSyncData> wormArcs) {
  this->wormArcs = wormArcs;
  std::vector<glm::vec4> colors;
  std::vector<float> ids;
  std::vector<float> timeData;
  for (WormArcSyncData wa : wormArcs) {
    colors.push_back(wa.getColor());
    ids.push_back(wa.getId());
    timeData.push_back(wa.getTime());
  }
  setWormColors(colors);
  setPlayerIds(ids);
  setArcTimeData(timeData);
  createVertices();
  createElements();
  update = true;
};

void RenderableWormArcs::setWormColors(std::vector<glm::vec4> wormColors) {
  this->wormColors = wormColors;
  vertexColorData.clear();
  for (int j = 0; j < wormColors.size(); ++j) {
    glm::vec4 color = wormColors.at(j);

    for (int i = 0; i < VERTS_PER_ARC; ++i) {
      vertexColorData.push_back(color[0]);
      vertexColorData.push_back(color[1]);
      vertexColorData.push_back(color[2]);
      vertexColorData.push_back(color[3]);
    }
  }

  update = true;
};

void RenderableWormArcs::setPlayerIds(std::vector<float> playerIds) {
  this->playerIds = playerIds;
  update = true;
};

void RenderableWormArcs::setArcTimeData(std::vector<float> arcTimeData) {
  this->arcTimeData = arcTimeData;
  update = true;
};

void RenderableWormArcs::createVertices() {
  cartesianVertexData.clear();
  arcFrontData.clear();
  arcBackData .clear();
  arcWidthData.clear();

  for (int j = 0; j < wormArcs.size(); ++j) {
    WormArcSyncData wa = wormArcs.at(j);
    glm::vec3 backPos = (glm::vec3) wa.getCartesianLerp(0.0);
    glm::vec3 midPos = (glm::vec3) wa.getCartesianLerp(0.5);
    glm::vec3 frontPos = (glm::vec3) wa.getCartesianLerp(1.0);

    glm::vec3 arcDirection = glm::normalize(frontPos - backPos);
    glm::vec3 domeNormal = glm::normalize(midPos);

    GLfloat arcWidth = wa.getWidth();

    for (int i = 0; i < VERTS_PER_ARC; ++i) {
      glm::vec3 quadPoint = midPos + glm::rotate(arcDirection * 0.05f, i*360.0f/VERTS_PER_ARC, domeNormal);

      cartesianVertexData.push_back(quadPoint.x);
      cartesianVertexData.push_back(quadPoint.y);
      cartesianVertexData.push_back(quadPoint.z);

      arcFrontData.push_back(frontPos.x);
      arcFrontData.push_back(frontPos.y);
      arcFrontData.push_back(frontPos.z);

      arcBackData.push_back(backPos.x);
      arcBackData.push_back(backPos.y);
      arcBackData.push_back(backPos.z);

      arcWidthData.push_back(arcWidth);
    }
  }
};

/**
 * Index the quads
 */
void RenderableWormArcs::createElements() {
  elementData.clear();
  for (int i = 0; i < wormArcs.size(); ++i) {
    int first = i*VERTS_PER_ARC;
    int second = i*VERTS_PER_ARC + 1;
    int third = i*VERTS_PER_ARC + 2;
    int fourth = i*VERTS_PER_ARC + 3;

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
void RenderableWormArcs::loadToGPU(bool sphericalCoords) {
  Renderable::loadToGPU(sphericalCoords);
  glBindVertexArray(vertexArray);

  GLuint vertexDim = 3;
  GLuint widthDim = 1;
  GLuint idDim = 1;
  GLuint timeDim = 1;

  glBindBuffer(GL_ARRAY_BUFFER, arcFrontBuffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    arcFrontData.size()*sizeof(GLfloat),
    arcFrontData.data(),
    GL_STATIC_DRAW
  );
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, vertexDim, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, arcBackBuffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    arcBackData.size()*sizeof(GLfloat),
    arcBackData.data(),
    GL_STATIC_DRAW
  );
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, vertexDim, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, arcWidthBuffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    arcWidthData.size()*sizeof(GLfloat),
    arcWidthData.data(),
    GL_STATIC_DRAW
  );
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, widthDim, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, playerIdBuffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    playerIds.size()*sizeof(GLfloat),
    playerIds.data(),
    GL_STATIC_DRAW
    );
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, idDim, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, arcTimeBuffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    arcTimeData.size()*sizeof(GLfloat),
    arcTimeData.data(),
    GL_STATIC_DRAW
    );
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, timeDim, GL_FLOAT, GL_FALSE, 0, 0);

  // Unbind
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);
  glDisableVertexAttribArray(4);
  glDisableVertexAttribArray(5);
  glDisableVertexAttribArray(6);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderableWormArcs::attach() {
  Renderable::attach();
  glGenBuffers(1, &arcFrontBuffer);
  glGenBuffers(1, &arcBackBuffer);
  glGenBuffers(1, &arcWidthBuffer);
  glGenBuffers(1, &playerIdBuffer);
  glGenBuffers(1, &arcTimeBuffer);
}

void RenderableWormArcs::detach() {
  glDeleteBuffers(1, &arcTimeBuffer);
  glDeleteBuffers(1, &playerIdBuffer);
  glDeleteBuffers(1, &arcWidthBuffer);
  glDeleteBuffers(1, &arcBackBuffer);
  glDeleteBuffers(1, &arcFrontBuffer);
  Renderable::detach();
}

void RenderableWormArcs::enableAttributes() {
  Renderable::enableAttributes();
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(4);
  glEnableVertexAttribArray(5);
  glEnableVertexAttribArray(6);
}

void RenderableWormArcs::disableAttributes() {
  glDisableVertexAttribArray(6);
  glDisableVertexAttribArray(5);
  glDisableVertexAttribArray(4);
  glDisableVertexAttribArray(3);
  glDisableVertexAttribArray(2);
  Renderable::disableAttributes();
}


GLuint RenderableWormArcs::getVertsPerElement() const {
  return VERTS_PER_ELEMENT;
};

const std::vector<WormArcSyncData> RenderableWormArcs::getWormArcs() const {
  return wormArcs;
};
