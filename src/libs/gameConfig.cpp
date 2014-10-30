#include <gameConfig.h>
#include <fstream>
#include <sstream>
#include <iostream>

GameConfig::GameConfig() {
  lineWidth = 0.05;
  maximumPlayers = 100;

  configEntities = {
    new ConfigEntity<double, float>("lineWidth", lineWidth),
    new ConfigEntity<double, int>("maximumPlayers", maximumPlayers),
    new ConfigEntity<std::string>("password", password)
  };

}

GameConfig::GameConfig(std::string configName) : GameConfig(){
  load(configName);
}

std::string GameConfig::toString() {
  JsonBuilder *builder = new JsonBuilder();
  for(auto entity : configEntities){
    entity->addConfigToJson(builder);
  }
  std::string result = builder->build();
  delete builder;
  return result;
}

void GameConfig::save(std::string configName){
  std::ofstream out(configName);
  if(!out){
    std::cerr << "Couldn't open file" << std::endl;
    return;
  }
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
    std::cerr << "Couldn't parse to JSON Object: " << err << std::endl;
    return;
  }
  for(auto entity : configEntities){
    entity->configFromJson(v);
  }
}

