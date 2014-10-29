#include "jsonBuilder.h"

JsonBuilder::JsonBuilder() {

}

JsonBuilder::~JsonBuilder() {

}

JsonBuilder* JsonBuilder::group() {
  return new JsonBuilder();
}

std::string JsonBuilder::prefix() {
  return "{";
}

std::string JsonBuilder::suffix() {
  return "}";
}

std::string JsonBuilder::separator() {
  return ",";
}

std::string JsonBuilder::formatKeyValue(std::string key, std::string value) {
  return "\"" + key + "\":" + value;
}

std::string JsonBuilder::formatVec3(glm::vec3 v) {
  std::stringstream ss;
  ss << "[" << v.x << separator() << v.y << separator() << v.z << "]";
  return ss.str();
}

std::string JsonBuilder::formatVec4(glm::vec4 v) {
  std::stringstream ss;
  ss << "[" << v.x << separator() << v.y << separator() << v.z << "]";
  return ss.str();
}

std::string JsonBuilder::formatString(std::string s) {
  return "\"" + s + "\"";
}

std::string JsonBuilder::formatInteger(int i) {
  return std::to_string(i);
}

std::string JsonBuilder::formatFloat(float f) {
  return std::to_string(f);
}