#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/constants.hpp>

#include <renderable.h>

class RenderableDome : public Renderable {
public:
  RenderableDome(int azmSegs, int inclSegs);
  ~RenderableDome();

  const int getAzmSegs() const;
  const int getInclSegs() const;
  const int getRadius() const;

  const GLuint getVertsPerElement() const;

protected:
  void createVertices();
  void createElements();

private:
  const float R = 1.0;
  int azmSegs, inclSegs;
  const GLuint VERTS_PER_ELEMENT = 3;
};
