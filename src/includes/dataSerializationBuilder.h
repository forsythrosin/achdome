#pragma once

#include <map>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class DataSerializationBuilder {
public:
  virtual ~DataSerializationBuilder() {};
  DataSerializationBuilder* add(std::string key, DataSerializationBuilder* value);
  DataSerializationBuilder* add(std::string key, std::string value);
  DataSerializationBuilder* add(std::string key, glm::vec3 value);
  DataSerializationBuilder* add(std::string key, glm::vec4 value);
  DataSerializationBuilder* add(std::string key, float value);
  DataSerializationBuilder* add(std::string key, int value);
  virtual DataSerializationBuilder* group() = 0;
  std::string build();
protected:
  std::map<std::string, DataSerializationBuilder*> builders;
  std::map<std::string, std::string> strings;
  std::map<std::string, glm::vec3> vec3s;
  std::map<std::string, glm::vec4> vec4s;
  std::map<std::string, float> floats;
  std::map<std::string, int> integers;
  virtual std::string prefix() = 0;
  virtual std::string suffix() = 0;
  virtual std::string separator() = 0;
  virtual std::string formatKeyValue(std::string key, std::string value) = 0;
  virtual std::string formatVec3(glm::vec3) = 0;
  virtual std::string formatVec4(glm::vec4) = 0;
  virtual std::string formatString(std::string) = 0;
  virtual std::string formatFloat(float) = 0;
  virtual std::string formatInteger(int) = 0;
};
