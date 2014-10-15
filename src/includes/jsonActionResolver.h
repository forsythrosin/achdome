#pragma once

#include "actionResolver.h"
#include <picojson/picojson.h>

class JsonActionResolver : public ActionResolver {
public:
	JsonActionResolver();
	~JsonActionResolver();
  bool resolve(std::string json, ClientAction &action);
private:
	bool getString(picojson::value, std::string, std::string&);
	bool getObject(picojson::value, std::string, picojson::value&);
};
