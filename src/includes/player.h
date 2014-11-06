#pragma once

#include <glm/glm.hpp>
#include <string>
#include <map>

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
  void setPointsInGame(int gameId, int points);
  int getPointsInGame(int gameId);
 private:
  int id;
  glm::vec4 color;
  char name[50];
  bool connected;
  std::map<int, int> points;
};
