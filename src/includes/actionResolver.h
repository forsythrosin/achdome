#pragma once

#include <map>

struct ClientAction {
  enum Type {
    START_GAME,
    END_GAME,
    AUTHENTICATE_ADMIN,
    UPDATE_SETTINGS,
    REGISTER,
    UNREGISTER,
    START_MOVING,
    LEFT_DOWN,
    LEFT_UP,
    RIGHT_DOWN,
    RIGHT_UP
  };
	Type type;
  std::map<std::string, std::string> strings;
  std::map<std::string, float> floats;
  std::map<std::string, int> ints;
};

class ActionResolver {
public:
	virtual ~ActionResolver() {};
  virtual bool resolve(std::string data, ClientAction &action) = 0;
};
