#pragma once

#include <renderable.h>
#include <wormArcSyncData.h>
#include <vector>

class RenderableWormArcs : public Renderable {
public:
  RenderableWormArcs(int wormCount, int segsPerWorm);
  ~RenderableWormArcs();

  const std::vector<WormArcSyncData> getWormArcs() const;

  void setWormArcs(std::vector<WormArcSyncData> wormArcs);
  void setWormColors(std::vector<glm::vec3> wormColors);
  void setWormColors(std::vector<glm::vec4> wormColors);
  void setPlayerIds(std::vector<float> playerIds);
  void setArcTimeData(std::vector<float> arcTimeData);

  GLuint getVertsPerElement() const;

  void loadToGPU(bool sphericalCoords = false);
  void attach();
  void detach();
  void enableAttributes();
  void disableAttributes();

protected:
  void createVertices();
  void createElements();

private:
  int vertsPerWorm, segsPerWorm;
  std::vector<WormArcSyncData> wormArcs;
  std::vector<glm::vec4> wormColors;
  std::vector<GLfloat> arcFrontData;
  std::vector<GLfloat> arcBackData;
  std::vector<GLfloat> arcWidthData;
  std::vector<GLfloat> playerIds;
  std::vector<GLfloat> arcTimeData;

  // additional GL handles
  GLuint arcFrontBuffer;
  GLuint arcBackBuffer;
  GLuint arcWidthBuffer;
  GLuint playerIdBuffer;
  GLuint arcTimeBuffer;

  const GLuint VERTS_PER_ELEMENT = 3;
  const GLuint ELEMENTS_PER_ARC = 2;
  const GLuint VERTS_PER_ARC = 4;
};
