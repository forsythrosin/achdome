#pragma once

#include <string>
struct GameConfig{

  static std::string pathToGameConfig(std::string filename){
    return "configs/gameConfigs/"+filename;
  }

  GameConfig();
  GameConfig(std::string);
  void save(std::string);
  void load(std::string);

  std::string toString();

  float lineWidth;
  float maximumPlayers;
};