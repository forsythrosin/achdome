#pragma once

#include "sgct/ogl_headers.h"
#include <vector>

// class Renderer;

class Renderable {
public:
  Renderable();
  virtual ~Renderable();

  const int getVertexCount() const;
  const int getElementCount() const;

  const std::vector<GLfloat> getCartesianVertexData() const;
  const std::vector<GLfloat> getSphericalVertexData() const;
  const std::vector<GLfloat> getVertexColorData() const;
  const std::vector<GLuint> getElementData() const;

  virtual void loadToGPU(bool sphericalCoords = false);
  virtual void attach();
  virtual void detach();
  virtual void enableAttributes();
  virtual void disableAttributes();

  virtual const GLuint getVertsPerElement() const = 0;

protected:
  virtual void createVertices() = 0;
  virtual void createElements() = 0;

  int vertexCount, elementCount;
  std::vector<GLfloat> cartesianVertexData, sphericalVertexData, vertexColorData;
  std::vector<GLuint> elementData;

  // GL handles
  GLuint vertexArray;
  GLuint positionBuffer, indexBuffer, colorBuffer;
  GLuint nextBufferIndex = 0;

  bool update = false;

friend class Renderer;
};
