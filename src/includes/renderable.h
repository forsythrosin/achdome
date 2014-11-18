#pragma once

#include "sgct/ogl_headers.h"
#include <glm/glm.hpp>
#include <vector>

// class Renderer;

class Renderable {
public:
  Renderable();
  virtual ~Renderable();

  int getVertexCount() const;
  int getElementCount() const;

  const std::vector<GLfloat> getCartesianVertexData() const;
  const std::vector<GLfloat> getSphericalVertexData() const;
  const std::vector<GLfloat> getVertexColorData() const;
  const std::vector<GLuint> getElementData() const;

  glm::mat4 getModelMatrix() const;
  void setModelMatrix(glm::mat4);

  virtual void loadToGPU(bool sphericalCoords = false);
  virtual void attach();
  virtual void detach();
  virtual void enableAttributes();
  virtual void disableAttributes();

  virtual GLuint getVertsPerElement() const = 0;

protected:
  virtual void createVertices() = 0;
  virtual void createElements() = 0;

  int vertexCount, elementCount;
  std::vector<GLfloat> cartesianVertexData, sphericalVertexData, vertexColorData;
  std::vector<GLuint> elementData;

  glm::mat4 modelMatrix = glm::mat4(1.0);

  // GL handles
  GLuint vertexArray;
  GLuint positionBuffer, indexBuffer, colorBuffer;
  GLuint nextBufferIndex = 0;

  bool update = false;

friend class Renderer;
};
