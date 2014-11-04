#pragma once

#include <renderable.h>
#include <wormHead.h>
#include <vector>

class RenderableWormHeads : public Renderable {
public:
  RenderableWormHeads(int wormCount, GLfloat wormThickness);
  ~RenderableWormHeads();

  const std::vector<WormHead> getWormHeads() const;

  void setWormHeads(std::vector<WormHead> wormHeads);
  void setWormColors(std::vector<glm::vec3> wormColors);
  void setWormColors(std::vector<glm::vec4> wormColors);

  const GLuint getVertsPerElement() const;

  void loadToGPU(bool sphericalCoords = false);
  void attach();
  void detach();

protected:
  void createVertices();
  void createElements();

private:
  GLfloat wormThickness;
  std::vector<WormHead> wormHeads;
  std::vector<glm::vec4> wormColors;
  GLfloat *headCenterData;

  // additional GL handles
  GLuint headCenterBuffer;

  const GLuint VERTS_PER_ELEMENT = 3;
  const GLuint ELEMENTS_PER_HEAD = 2;
  const GLuint VERTS_PER_HEAD = 4;
};
