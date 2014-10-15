#pragma once

#include "sgct/ogl_headers.h"

class Renderable {
public:
  Renderable() = default;
  virtual ~Renderable();

  const int getVertexCount() const;
  const int getElementCount() const;

  const GLfloat * const getCartesianVertexData() const;
  const GLfloat * const getSphericalVertexData() const;
  const GLuint * const getElementData() const;

  virtual const GLuint getVertsPerElement() const = 0;

protected:
  virtual void createVertices() = 0;
  virtual void createElements() = 0;

  int vertexCount, elementCount;
  GLfloat *cartesianVertexData, *sphericalVertexData;
  GLuint *elementData;
};
