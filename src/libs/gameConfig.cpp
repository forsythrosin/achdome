#include <gameConfig.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "sgct.h"

GameConfig::GameConfig() {
  // Default values if there is no config file
  wormWidth = 0.005;
  wormSpeed = 0.002;
  turnSpeed = 0.1;
  countdown = 10;
  maximumPlayers = 100;
  countdownDuration = 5;
  gameOverDuration = 5;
  password = "default";
  minTimeBetweenGaps = 200;
  maxTimeBetweenGaps = 400;
  minTimeInGap = 10;
  maxTimeInGap = 20;

  configEntities = {
    new ConfigEntity<double, float>("wormWidth", wormWidth),
    new ConfigEntity<double, float>("wormSpeed", wormSpeed),
    new ConfigEntity<double, float>("turnSpeed", turnSpeed),
    new ConfigEntity<double, int>("countdown", countdown),
    new ConfigEntity<double, int>("maximumPlayers", maximumPlayers),
    new ConfigEntity<std::string>("password", password),
    new ConfigEntity<double, float>("countdownDuration", countdownDuration),
    new ConfigEntity<double, float>("gameOverDuration", gameOverDuration),
    new ConfigEntity<double, int>("minTimeBetweenGaps", minTimeBetweenGaps),
    new ConfigEntity<double, int>("maxTimeBetweenGaps", maxTimeBetweenGaps),
    new ConfigEntity<double, int>("minTimeInGap", minTimeInGap),
    new ConfigEntity<double, int>("maxTimeInGap", maxTimeInGap)
  };

}

GameConfig::GameConfig(std::string configName) : GameConfig(){
  load(configName);
}

GameConfig::~GameConfig() {
  for(auto ce : configEntities){
    delete ce;
  }
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
  parse(configString);
}

void GameConfig::encode() {
  sgct::SharedString str;
  str.setVal(toString());
  sgct::SharedData::instance()->writeString(&str);
}

void GameConfig::parse(std::string string) {
  std::string err;

  picojson::value v;
  picojson::parse(v, string.begin(), string.end(), &err);

  if(!err.empty() || !v.is<picojson::object>()){
    std::cerr << "Couldn't parse to JSON Object: " << err << std::endl;
    return;
  }
  for(auto entity : configEntities){
    entity->configFromJson(v);
  }
}

void GameConfig::decode() {
  sgct::SharedString str;
  sgct::SharedData::instance()->readString(&str);
  parse(str.getVal());
}
