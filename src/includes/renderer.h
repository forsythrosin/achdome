#pragma once

#include "sgct.h"
#include <string>
#include <vector>
#include <map>
#include <renderable.h>

class RenderConfig;
class AbstractUniform;

class Renderer {
public:
  Renderer(sgct::Engine *gEngine);
  ~Renderer();

  // Renderables handling
  int addRenderable(Renderable *renderable, GLenum mode, std::string vert, std::string frag, bool spherical);
  void removeRenderable(int configId);

  // Render methods
  void render(int configId, int configWithFBOId = -1, int stitchStep = 0);
  void renderToFBO(int configId, int stitchStep = 0);

  // Convenience methods
  void setUniform(int configId, AbstractUniform *uniform);
  void resetFBO(int configId);
  glm::mat4 getMVP();

private:
  void init(RenderConfig &renderConfig);

  std::map<int, RenderConfig> renderConfigs;
  float rot = 0.0f;
  sgct::Engine *gEngine;
  int nextId = 0;

  const float DOME_ROTATION = -90.0;
  const glm::mat4 ROT_STAT = glm::rotate(glm::mat4(1.0f), DOME_ROTATION, glm::vec3(1.0f, 0.0f, 0.0f));
};
