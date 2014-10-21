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
  virtual DataSerializationBuilder* group() = 0;
  std::string build();
protected:
  std::map<std::string, DataSerializationBuilder*> builders;
  std::map<std::string, std::string> strings;
  std::map<std::string, glm::vec3> vec3s;
  virtual std::string prefix() = 0;
  virtual std::string suffix() = 0;
  virtual std::string separator() = 0;
  virtual std::string keyValue(std::string key, std::string value) = 0;
  virtual std::string vec3(glm::vec3) = 0;
  virtual std::string string(std::string) = 0;
};
