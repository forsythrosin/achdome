#pragma once

#include "dataSerializationBuilder.h"
#include <sstream>

class JsonBuilder : public DataSerializationBuilder {
public:
  JsonBuilder();
  ~JsonBuilder();
  JsonBuilder* group();
protected:
  std::string prefix();
  std::string suffix();
  std::string separator();
  std::string formatKeyValue(std::string key, std::string value);
  std::string formatVec3(glm::vec3);
  std::string formatVec4(glm::vec4);
  std::string formatString(std::string);
  std::string formatInteger(int);
  std::string formatFloat(float);
};
