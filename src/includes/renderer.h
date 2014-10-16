#pragma once

#include "sgct.h"
#include <renderable.h>
#include <string>
#include <vector>
#include <FBO.h>
#include <texture2D.h>

struct RenderConfig {
  RenderConfig(Renderable *renderable, GLenum mode, std::string vert, std::string frag, bool sphericalCoords) {
    this->renderable = renderable;
    this->mode = mode;
    this->vertShader = vert;
    this->fragShader = frag;
    this->sphericalCoords = sphericalCoords;
  };

  Renderable *renderable;
  GLenum mode;
  std::string vertShader;
  std::string fragShader;
  bool sphericalCoords;

  int id;

  GLuint VBO = GL_FALSE;
  GLuint IBO = GL_FALSE;
  GLuint VAO = GL_FALSE;
  GLint matrixLocation = -1;
  GLint textureLocation = -1;
  GLint fboTexSizeLocation = -1;

  std::vector<FBO*> framebuffers;
};

class Renderer {
public:
  Renderer(sgct::Engine *gEngine);
  ~Renderer() = default;
  int addRenderable(Renderable *renderable, GLenum mode, std::string vert, std::string frag, bool spherical);
  void render(int configId, int configWithFBOId = -1, int stitchStep = 0);
  void renderToFBO(int configId, int stitchStep = 0);

private:
  void init(RenderConfig &renderConfig);
  void loadToGPU(RenderConfig &renderConfig);
  std::vector<RenderConfig> renderConfigs;
  const float DOME_ROTATION = -117.0;
  float rot = 0.0f;
  sgct::Engine *gEngine;
};
