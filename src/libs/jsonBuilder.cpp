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

std::string JsonBuilder::keyValue(std::string key, std::string value) {
  return "\"" + key + "\":" + value;
}

std::string JsonBuilder::vec3(glm::vec3 v) {
  std::stringstream ss;
  ss << "[" << v.x << separator() << v.y << separator() << v.z << "]";
  return ss.str();
}

std::string JsonBuilder::string(std::string s) {
  return "\"" + s + "\"";
}