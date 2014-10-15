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
  // void setWormArc(int playerId);

  const GLuint getVertsPerElement() const;

protected:
  void createVertices();
  void createElements();

private:
  int vertsPerWorm, segsPerWorm;
  std::vector<WormArc> wormArcs;
  const GLuint VERTS_PER_ELEMENT = 2;
};
