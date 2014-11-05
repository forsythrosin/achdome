#pragma once

#include <renderable.h>
#include <wormArc.h>
#include <vector>

class RenderableWormArcs : public Renderable {
public:
  RenderableWormArcs(int wormCount, int segsPerWorm);
  ~RenderableWormArcs();

  const std::vector<WormArc> getWormArcs() const;

  void setWormArcs(std::vector<WormArc> wormArcs);
  void setWormColors(std::vector<glm::vec3> wormColors);
  void setWormColors(std::vector<glm::vec4> wormColors);

  const GLuint getVertsPerElement() const;

protected:
  void createVertices();
  void createElements();

private:
  int vertsPerWorm, segsPerWorm;
  std::vector<WormArc> wormArcs;
  std::vector<glm::vec4> wormColors;
  const GLuint VERTS_PER_ELEMENT = 3;
};
