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
  std::vector<WormHead> wormHeads;
  std::vector<glm::vec4> wormColors;
  std::vector<GLfloat> headCenterData;
  std::vector<GLfloat> headDirectionData;

  // additional GL handles
  GLuint headCenterBuffer;
  GLuint headDirectionBuffer;

  const GLuint VERTS_PER_ELEMENT = 3;
  const GLuint ELEMENTS_PER_HEAD = 2;
  const GLuint VERTS_PER_HEAD = 4;
};
