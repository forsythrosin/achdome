#pragma once

#include "sgct/ogl_headers.h"

// class Renderer;

class Renderable {
public:
  Renderable();
  virtual ~Renderable();

  const int getVertexCount() const;
  const int getElementCount() const;

  const GLfloat * const getCartesianVertexData() const;
  const GLfloat * const getSphericalVertexData() const;
  const GLfloat * const getVertexColorData() const;
  const GLuint * const getElementData() const;

  virtual void loadToGPU(bool sphericalCoords = false);
  virtual void attach();
  virtual void detach();

  virtual const GLuint getVertsPerElement() const = 0;

protected:
  virtual void createVertices() = 0;
  virtual void createElements() = 0;

  int vertexCount, elementCount;
  GLfloat *cartesianVertexData, *sphericalVertexData, *vertexColorData;
  GLuint *elementData;

  // GL handles
  GLuint vertexArray;
  GLuint positionBuffer, indexBuffer, colorBuffer;

  bool update = false;

friend class Renderer;
};
