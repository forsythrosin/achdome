#include "dataSerializationBuilder.h"

DataSerializationBuilder* DataSerializationBuilder::add(std::string key, DataSerializationBuilder *value) {
  builders.insert({ key, value });
  return this;
}

DataSerializationBuilder* DataSerializationBuilder::add(std::string key, std::string value) {
  strings.insert({ key, value });
  return this;
}

DataSerializationBuilder* DataSerializationBuilder::add(std::string key, glm::vec3 value) {
  vec3s.insert({ key, value });
  return this;
}

std::string DataSerializationBuilder::build() {
  std::string str = prefix();
  bool sep = false;
  for (std::pair<std::string, DataSerializationBuilder*> b : builders) {
    if (sep || !(sep = true)) str += separator();
    str += keyValue(b.first, b.second->build());
    delete b.second;
  }
  for (std::pair<std::string, glm::vec3> v : vec3s) {
    if (sep || !(sep = true)) str += separator();
    str += keyValue(v.first, vec3(v.second));
  }
  for (std::pair<std::string, std::string> s : strings) {
    if (sep || !(sep = true)) str += separator();
    str += keyValue(s.first, string(s.second));
  }
  str += suffix();
  builders.clear();
  strings.clear();
  vec3s.clear();
  return str;
}