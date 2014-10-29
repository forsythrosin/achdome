#include <gameConfig.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <picojson/picojson.h>
#include <jsonBuilder.h>

GameConfig::GameConfig() {
  lineWidth = 0.05;
  maximumPlayers = 100;
}

GameConfig::GameConfig(std::string configName) {
  load(configName);
}

std::string GameConfig::toString() {
  JsonBuilder *builder = new JsonBuilder();
  builder->add("lineWidth", lineWidth)
         ->add("maximumPlayers", maximumPlayers);
  return builder->build();
}

void GameConfig::save(std::string configName){
  std::ofstream out(pathToGameConfig(configName));
  out << toString();
  out.close();
}

void GameConfig::load(std::string configName){
  std::ifstream in(pathToGameConfig(configName), std::ios::in | std::ios::binary);
  std::string configString;
  if (!in) {
    std::cerr << "Couldn't open file" << std::endl;
    return;
  }

  std::ostringstream contents;
  contents << in.rdbuf();
  in.close();
  configString = contents.str();
  std::string err;
  picojson::value v;
  picojson::parse(v, configString.begin(), configString.end(), &err);

  if(!err.empty() || !v.is<picojson::object>()){
    std::cerr << "Couldn't parse to JSON Object" << std::endl;
    return;
  }

  if(v.contains("lineWidth")){
    lineWidth = (float)v.get("lineWidth").get<double>();
  }

  if(v.contains("maximumPlayers")){
    maximumPlayers = (int)v.get("maximumPlayers").get<double>();
  }

}
