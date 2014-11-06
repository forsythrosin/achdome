#pragma once

#include "sgct.h"
#include <string>
#include <vector>
#include <map>
#include <texture2D.h>
#include <renderable.h>
#include <FBO.h>

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
  bool hidden = false;

  // uniform
  GLint matrixLocation = -1;
  GLint textureLocation = -1;
  GLint fboTexSizeLocation = -1;
  GLint timeLocation = -1;

  std::vector<FBO*> framebuffers;
};

class Renderer {
public:
  Renderer(sgct::Engine *gEngine);
  ~Renderer();
  int addRenderable(Renderable *renderable, GLenum mode, std::string vert, std::string frag, bool spherical);
  void removeRenderable(int configId);
  void render(int configId, int configWithFBOId = -1, int stitchStep = 0);
  void renderToFBO(int configId, int stitchStep = 0);
  void resetFBO(int configId);
  
  glm::mat4 getMVP();

private:
  void init(RenderConfig &renderConfig);

  std::map<int, RenderConfig> renderConfigs;
  const float DOME_ROTATION = -90.0;
  float rot = 0.0f;
  sgct::Engine *gEngine;
  int nextId = 0;
  const glm::mat4 ROT_STAT = glm::rotate(glm::mat4(1.0f), DOME_ROTATION, glm::vec3(1.0f, 0.0f, 0.0f));
};
