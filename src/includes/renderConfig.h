#pragma once

#include "sgct.h"
#include <string>
#include <vector>
#include <map>
#include <texture2D.h>
#include <renderable.h>
#include <FBO.h>
#include <uniformWrappers.h>

class RenderConfig {
private:
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

  // default uniforms
  GLuint matrixLocation = -1;
  GLuint textureLocation = -1;
  GLuint fboTexSizeLocation = -1;

  std::map<std::string, AbstractUniform* > uniforms;
  std::vector<FBO*> framebuffers;

friend class Renderer;
};