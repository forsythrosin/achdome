#include <renderableDome.h>

RenderableDome::RenderableDome(int as, int is) {
  azmSegs = as;
  inclSegs = is;

  vertexCount = azmSegs*(inclSegs+1); // e.g. 4 verts needed to draw 3 segs
  elementCount = azmSegs*inclSegs*2;

  sphericalVertexData.reserve(vertexCount*2);
  cartesianVertexData.reserve(vertexCount*3);
  vertexColorData.reserve(vertexCount*4);
  elementData.reserve(elementCount*VERTS_PER_ELEMENT);

  createVertices();
  createElements();
};

RenderableDome::~RenderableDome() {};

/**
 * Vertex placement
 */
void RenderableDome::createVertices() {
  cartesianVertexData.clear();
  sphericalVertexData.clear();
  for (int j = 0; j <= inclSegs; ++j) {
    float theta = glm::half_pi<float>()*(float)j/inclSegs;
    for (int i = 0; i < azmSegs; ++i) {
      float phi = 2.0*glm::pi<float>()*(float)i/azmSegs;
      int cartIdx = (j*azmSegs + i)*3;
      int sphereIdx = (j*azmSegs + i)*2;

      // (θ, φ) to (x, y, z)
      // x
      cartesianVertexData.push_back(R*glm::sin(theta)*glm::cos(phi));
      // y
      cartesianVertexData.push_back(R*glm::sin(theta)*glm::sin(phi));
      // z
      cartesianVertexData.push_back(R*glm::cos(theta));

      sphericalVertexData.push_back(phi);
      sphericalVertexData.push_back(theta);
    }
  }
};

/**
 * Tessellation of the dome
 */
void RenderableDome::createElements() {
  elementData.clear();
  for (int i = 0; i < azmSegs*inclSegs; ++i) {
    int idx = i*6;
    int rowIndex = i%azmSegs;

    // vertices
    int self = i;
    int straightDown = i + azmSegs;
    int prev = rowIndex ? i - 1 : i + azmSegs - 1;
    int nextDown = rowIndex == (azmSegs - 1) ? i + 1 : i + azmSegs + 1;

    // left element
    elementData.push_back(self);
    elementData.push_back(straightDown);
    elementData.push_back(prev);

    // right element
    elementData.push_back(self);
    elementData.push_back(nextDown);
    elementData.push_back(straightDown);
  }
};

int RenderableDome::getAzmSegs() const {
  return azmSegs;
};

int RenderableDome::getInclSegs() const {
  return inclSegs;
};

int RenderableDome::getRadius() const {
  return R;
};

GLuint RenderableDome::getVertsPerElement() const {
  return VERTS_PER_ELEMENT;
};
