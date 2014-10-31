#pragma once

#include <string>
#include <picojson/picojson.h>
#include <jsonBuilder.h>

struct GameConfig{

  static std::string pathToGameConfig(std::string filename){
    return "configs/gameConfigs/"+filename;
  }

  GameConfig();
  GameConfig(std::string);
  ~GameConfig();
  void save(std::string);
  void load(std::string);
  void encode();
  void decode();
  std::string toString();

  float wormWidth;
  float wormSpeed;
  float turnSpeed;
  int countdown;

  void parse(std::string);
  int maximumPlayers;
  std::string password;
  float countdownDuration;
  float gameOverDuration;

  private:

  struct AbstractConfigEntity{
    virtual void configFromJson(picojson::value) = 0;
    virtual void addConfigToJson(JsonBuilder*) = 0;
  };

  template<typename T, typename U = T>
  class ConfigEntity : public AbstractConfigEntity{
    public:

    typedef T type;
    ConfigEntity(std::string key, U &value) : key(key), value(value) {}

    void configFromJson(picojson::value obj){
      if(obj.contains(key)){
        value = (U) obj.get(key).get<type>();
      }
    }

    void addConfigToJson(JsonBuilder *builder){
      builder->add(key, value);
    }

    private:
    std::string key;
    U &value;
  };

  std::vector< AbstractConfigEntity* > configEntities;

};
