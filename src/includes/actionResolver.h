#pragma once

#include <map>

enum ActionType {
	REGISTER,
	START,
	LEFT_DOWN,
	LEFT_UP,
	RIGHT_DOWN,
	RIGHT_UP
};

struct Action {
	ActionType type;
	std::map<std::string, std::string> data;
};

class ActionResolver {
public:
	virtual ~ActionResolver() {};
	virtual bool resolve(std::string data, Action &action) = 0;
};