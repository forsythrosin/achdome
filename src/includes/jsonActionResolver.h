#pragma once

#include "actionResolver.h"
#include <picojson/picojson.h>

class JsonActionResolver : public ActionResolver {
public:
	JsonActionResolver();
	~JsonActionResolver();
	bool resolve(std::string json, Action &action);
};