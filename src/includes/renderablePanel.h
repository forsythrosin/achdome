#pragma once

#include <glm/glm.hpp>
#include <renderable.h>
#include <textTexture2D.h>

class RenderablePanel : public Renderable {
public:
  RenderablePanel(glm::vec3 pos, float width, float height);
  ~RenderablePanel();

  void setPos(glm::vec3 pos);
  void setWidth(float width);
  void setHeight(float height);
  void setAnchor(glm::vec3 anchor);
  void setColor(glm::vec4 color);

  void enableAttributes();
  void disableAttributes();
  void attach();
  void detach();
  
  void loadToGPU(bool sphericalCoords);

  glm::vec3 getPos() const;
  float getWidth() const;
  float getHeight() const;
  GLuint getVertsPerElement() const;
  glm::vec3 getAnchor() const;

protected:
  void createVertices();
  void createElements();

private:
  float width, height;
  glm::vec3 pos, anchor;

  std::vector<GLfloat> textureCoordinates;

  GLuint textureCoordBuffer;
  
  
  const int VERTEX_COUNT = 4;
  const int ELEMENT_COUNT = 2;
  const glm::vec3 UI_PLANE_POS = glm::vec3(0.0, 7.5, 0.0);
  const GLuint VERTS_PER_ELEMENT = 3;
};
