#pragma once

#include "sgct.h"
#include <renderer.h>

class RenderState {
 public:
  RenderState(sgct::Engine *gEngine) : gEngine(gEngine){
    renderer = new Renderer(gEngine);
  };
  virtual ~RenderState() {
    std::cout << "RenderState destructor called" << std::endl;
    delete renderer;
  };

  virtual void init() = 0;
  virtual void preSync() = 0;
  virtual void draw() = 0;
  virtual void encode() = 0;
  virtual void decode() = 0;

  bool isActive() { return active; }

 protected:
  bool active;
  Renderer *renderer;
  sgct::Engine *gEngine;
};
