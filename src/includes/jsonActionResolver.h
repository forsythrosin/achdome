#pragma once

#include "actionResolver.h"
#include <picojson/picojson.h>

class JsonActionResolver : public ActionResolver {
public:
	JsonActionResolver();
	~JsonActionResolver();
  bool resolve(std::string json, ClientAction &action);
private:
  bool getInt(picojson::value, std::string, int&);
  bool getFloat(picojson::value, std::string, float&);
	bool getString(picojson::value, std::string, std::string&);
  bool getObject(picojson::value, std::string, picojson::value&);

  bool getDataInt(picojson::value, std::string, ClientAction&);
  bool getDataFloat(picojson::value, std::string, ClientAction&);
  bool getDataString(picojson::value, std::string, ClientAction&);
};
