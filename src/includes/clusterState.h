#pragma once

#include "sgct.h"
#include <renderer.h>

class ClusterState {
 public:
  ClusterState(sgct::Engine *gEngine) : gEngine(gEngine){
    renderer = new Renderer(gEngine);
  };
  virtual ~ClusterState() {
    std::cout << "ClusterState destructor called" << std::endl;
    delete renderer;
  };

  virtual void attach() = 0;
  virtual void detach() = 0;

  virtual void preSync() = 0;
  virtual void draw() = 0;
  virtual void encode() = 0;
  virtual void decode() = 0;

  bool isAttached() { return attached; }

 protected:
  bool attached;
  Renderer *renderer;
  sgct::Engine *gEngine;
};
