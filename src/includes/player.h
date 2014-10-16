#pragma once

#include <glm/glm.hpp>
#include <string>

class Player {
 public:
  Player(int id, glm::vec3 color, std::string name);
  int getId();
  void setName(std::string name);
  std::string getName();
  bool isConnected();
  bool disconnect();
 private:
  int id;
  glm::vec3 color;
  std::string name;
  bool connected;
};
