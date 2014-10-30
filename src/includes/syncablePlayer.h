#pragma once

#include <glm/glm.hpp>

class SyncablePlayer {
 public:
  SyncablePlayer(int id, glm::vec4 color) : id(id), color(color) {};
  void setId(int id) {this->id = id;};
  void setColor(glm::vec4 color) {this->color = color;};

  int getId() { return id;};
  glm::vec4 getColor() {return color;};
 private:
  int id;
  glm::vec4 color;
};
