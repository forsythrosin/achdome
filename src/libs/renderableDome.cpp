#include <renderableDome.h>

RenderableDome::RenderableDome(int as, int is) {
  azmSegs = as;
  inclSegs = is;

  vertexCount = azmSegs*inclSegs;
  elementCount = azmSegs*(inclSegs-1)*2;

  sphericalVertexData = new GLfloat[vertexCount*2];
  cartesianVertexData = new GLfloat[vertexCount*3];
  vertexColorData = new GLfloat[vertexCount*4];
  elementData = new GLuint[elementCount*VERTS_PER_ELEMENT];

  createVertices();
  createElements();
};

RenderableDome::~RenderableDome() {};

/**
 * Vertex placement
 */
void RenderableDome::createVertices() {
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
void RenderableDome::createElements() {
  for (int i = 0; i < azmSegs*(inclSegs-1); ++i) {
    int idx = i*6;
    int rowIndex = i%azmSegs;

    // vertices
    int self = i;
    int straightDown = i + azmSegs;
    int prev = rowIndex ? i - 1 : i + azmSegs - 1;
    int nextDown = rowIndex == (azmSegs - 1) ? i + 1 : i + azmSegs + 1;

    // left element
    elementData[idx] = self;
    elementData[idx + 1] = straightDown;
    elementData[idx + 2] = prev;

    // right element
    elementData[idx + 3] = self;
    elementData[idx + 4] = nextDown;
    elementData[idx + 5] = straightDown;
  }
};

const int RenderableDome::getAzmSegs() const {
  return azmSegs;
};

const int RenderableDome::getInclSegs() const {
  return inclSegs;
};

const int RenderableDome::getRadius() const {
  return R;
};

const GLuint RenderableDome::getVertsPerElement() const {
  return VERTS_PER_ELEMENT;
};
