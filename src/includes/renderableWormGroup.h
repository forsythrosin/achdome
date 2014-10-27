#pragma once

#include <renderable.h>
#include <wormArc.h>
#include <vector>

class RenderableWormGroup : public Renderable {
public:
  RenderableWormGroup(int wormCount, int segsPerWorm);
  ~RenderableWormGroup();

  const std::vector<WormArc> getWormArcs() const;

  void setWormArcs(std::vector<WormArc> wormArcs);
  void setWormColors(std::vector<glm::vec4> wormColors);

  const GLuint getVertsPerElement() const;

protected:
  void createVertices();
  void createElements();

private:
  int vertsPerWorm, segsPerWorm;
  std::vector<WormArc> wormArcs;
  std::vector<glm::vec4> wormColors;
  const GLuint VERTS_PER_ELEMENT = 2;
};
