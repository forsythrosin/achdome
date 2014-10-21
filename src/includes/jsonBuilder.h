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
  std::string keyValue(std::string key, std::string value);
  std::string vec3(glm::vec3);
  std::string string(std::string);
};
