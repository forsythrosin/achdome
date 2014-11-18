#pragma once

#include <glm/glm.hpp>
#include <renderable.h>
#include <textTexture2D.h>

class RenderablePanel : public Renderable {
public:
  RenderablePanel();
  ~RenderablePanel();

  void setColor(glm::vec4 color);

  void enableAttributes();
  void disableAttributes();
  void attach();
  void detach();

  void loadToGPU(bool sphericalCoords);

  GLuint getVertsPerElement() const;

protected:
  void createVertices();
  void createVertices(glm::vec3 posTL, glm::vec3 posTR, glm::vec3 posBL, glm::vec3 posBR);
  void createElements();

private:
  std::vector<GLfloat> textureCoordinates;

  GLuint textureCoordBuffer;

  const int VERTEX_COUNT = 4;
  const int ELEMENT_COUNT = 2;
  const GLuint VERTS_PER_ELEMENT = 3;
};
