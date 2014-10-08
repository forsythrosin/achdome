#include <domeSurface.h>

DomeSurface::DomeSurface(int as, int is) {
  azmSegs = as;
  inclSegs = is;

  vertexCount = azmSegs*inclSegs;
  triangleCount = azmSegs*(inclSegs-1)*2;

  sphericalVertexData = new GLfloat[vertexCount*2];
  cartesianVertexData = new GLfloat[vertexCount*3];
  triangleData = new GLuint[triangleCount*3];

  createVertices();
  createTriangles();
};

DomeSurface::~DomeSurface() {
  delete [] sphericalVertexData;
  delete [] cartesianVertexData;
  delete [] triangleData;
};

/**
 * Vertex placement
 */
void DomeSurface::createVertices() {
  for (int j = 0; j < inclSegs; ++j) {
    float theta = glm::half_pi<float>()*(float)j/inclSegs;
    for (int i = 0; i < azmSegs; ++i) {
      float phi = 2.0*glm::pi<float>()*(float)i/azmSegs;
      int cartIdx = (j*azmSegs + i)*3;
      int sphereIdx = (j*azmSegs + i)*2;

      // (θ, φ) to (x, y, z)
      // x
      cartesianVertexData[cartIdx] = R*glm::sin(theta)*glm::cos(phi);
      // y
      cartesianVertexData[cartIdx + 1] = R*glm::sin(theta)*glm::sin(phi);
      // z
      cartesianVertexData[cartIdx + 2] = R*glm::cos(theta);

      sphericalVertexData[sphereIdx] = phi;
      sphericalVertexData[sphereIdx + 1] = theta;
    }
  }
};

/**
 * Tessellation of the dome
 */
void DomeSurface::createTriangles() {
  for (int i = 0; i < azmSegs*(inclSegs-1); ++i) {
    int idx = i*6;
    int rowIndex = i%azmSegs;

    // vertices
    int self = i;
    int straightDown = i + azmSegs;
    int prev = rowIndex ? i - 1 : i + azmSegs - 1;
    int nextDown = rowIndex == (azmSegs - 1) ? i + 1 : i + azmSegs + 1;

    // left triangle
    triangleData[idx] = self;
    triangleData[idx + 1] = straightDown;
    triangleData[idx + 2] = prev;

    // right triangle
    triangleData[idx + 3] = self;
    triangleData[idx + 4] = nextDown;
    triangleData[idx + 5] = straightDown;
  }
};

const int DomeSurface::getAzmSegs() const {
  return azmSegs;
};

const int DomeSurface::getInclSegs() const {
  return inclSegs;
};

const int DomeSurface::getRadius() const {
  return R;
};

const GLfloat * const DomeSurface::getCartesianVertexData() const {
  return cartesianVertexData;
};

const GLfloat * const DomeSurface::getSphericalVertexData() const {
  return sphericalVertexData;
};

const GLuint * const DomeSurface::getTriangleData() const {
  return triangleData;
};

const int DomeSurface::getVertexCount() const {
  return vertexCount;
};

const int DomeSurface::getTriangleCount() const {
  return triangleCount;
};
