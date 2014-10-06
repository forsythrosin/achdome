#include <glm/glm.hpp>
#include <glm/gtx/constants.hpp>
#include "sgct/ogl_headers.h"

class DomeSurface {
public:
  DomeSurface(int azmSegs, int inclSegs);
  const int getAzmSegs() const;
  const int getInclSegs() const;
  const int getRadius() const;

  const int getVertexCount() const;
  const int getTriangleCount() const;

  const GLfloat * const getCartesianVertexData() const;
  const GLfloat * const getSphericalVertexData() const;
  const GLuint * const getTriangleData() const;

private:
  // Instantiation/tesselation helpers, private for now
  void createVertices();
  void createTriangles();

  const float R = 1.0;
  int azmSegs, inclSegs, vertexCount, triangleCount;
  GLfloat *cartesianVertexData, *sphericalVertexData;
  GLuint *triangleData;
};
