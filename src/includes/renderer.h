#pragma once

#include "sgct.h"
#include <renderable.h>
#include <string>
#include <vector>


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
  GLuint VAO;
  GLint Matrix_Loc = -1;
};

class Renderer {
public:
  Renderer(sgct::Engine *gEngine);
  ~Renderer() = default;
  int addRenderable(Renderable *renderable, GLenum mode, std::string vert, std::string frag, bool spherical);
  void render(int configId);
  void renderAll();

private:
  void init(RenderConfig &renderConfig);
  void loadToGPU(RenderConfig &renderConfig);
  std::vector<RenderConfig> renderConfigs;
  const float DOME_ROTATION = -117.0;
  float rot = 0.0f;
  sgct::Engine *gEngine;
};
