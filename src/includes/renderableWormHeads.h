#pragma once

#include <renderable.h>
#include <wormHeadSyncData.h>
#include <vector>

class RenderableWormHeads : public Renderable {
public:
  RenderableWormHeads(int wormCount, GLfloat wormThickness);
  ~RenderableWormHeads();

  const std::vector<WormHeadSyncData> getWormHeads() const;

  void setWormHeads(std::vector<WormHeadSyncData> wormHeads);
  void setWormColors(std::vector<glm::vec3> wormColors);
  void setWormColors(std::vector<glm::vec4> wormColors);

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
  GLfloat wormThickness;
  std::vector<WormHeadSyncData> wormHeads;
  std::vector<glm::vec4> wormColors;
  std::vector<GLfloat> headCenterData;
  std::vector<GLfloat> headDirectionData;
  std::vector<GLfloat> appearanceData;

  // additional GL handles
  GLuint headCenterBuffer;
  GLuint headDirectionBuffer;
  GLuint appearanceBuffer;

  const GLuint VERTS_PER_ELEMENT = 3;
  const GLuint ELEMENTS_PER_HEAD = 2;
  const GLuint VERTS_PER_HEAD = 4;
};
