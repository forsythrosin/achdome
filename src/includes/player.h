#pragma once

#include <glm/glm.hpp>
#include <string>

class Player {
 public:
  Player(int id, glm::vec4 color, std::string name);
  int getId();
  void setName(std::string name);
  std::string getName();
  void setColor(glm::vec4 color);
  glm::vec4 getColor();
  bool isConnected();
  bool disconnect();
 private:
  int id;
  glm::vec4 color;
  std::string name;
  bool connected;
};
