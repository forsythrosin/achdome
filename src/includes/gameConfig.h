#pragma once

#include <string>
#include <picojson/picojson.h>

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
  int maximumPlayers;

  private:
  struct AbstractConfigEntity{
    virtual void configFromJson(picojson::value) = 0;
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
    private:
    std::string key;
    U &value;
  };


};