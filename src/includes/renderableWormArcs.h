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

  GLuint getVertsPerElement() const;

protected:
  void createVertices();
  void createElements();

private:
  int vertsPerWorm, segsPerWorm;
  std::vector<WormArcSyncData> wormArcs;
  std::vector<glm::vec4> wormColors;
  const GLuint VERTS_PER_ELEMENT = 3;
};
