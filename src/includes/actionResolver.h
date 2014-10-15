#pragma once

#include <map>

struct Action {
  enum Type {
    REGISTER,
    UNREGISTER,
    START,
    LEFT_DOWN,
    LEFT_UP,
    RIGHT_DOWN,
    RIGHT_UP
  };
	Type type;
	std::map<std::string, std::string> data;
};

class ActionResolver {
public:
	virtual ~ActionResolver() {};
	virtual bool resolve(std::string data, Action &action) = 0;
};