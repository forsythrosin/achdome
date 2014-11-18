#pragma once

#include "sgct.h"
#include <texture2D.h>

class AbstractUniform {
public:
  AbstractUniform(std::string n) : name(n), location(-1) {};
  virtual ~AbstractUniform(){};
  virtual void upload() = 0;
  std::string getName() { return name; };
  void setLocation(GLint l) { location = l; };
  GLint getLocation() { return location; };
  bool needsUpdate = false;

protected:
  std::string name;
  GLint location;
};

template <typename T>
class TemplateUniform : public AbstractUniform {
public:
  TemplateUniform(std::string n) : AbstractUniform(n), value(T()) {};
  void set(T t) { value = t; };
  virtual ~TemplateUniform(){};
protected:
  T value;
};

template <typename T>
class Uniform : public TemplateUniform<T> {};

/***************************
* Concrete implementations *
****************************/

template <>
class Uniform <float> : public TemplateUniform <float> {
public:
  Uniform(std::string n) : TemplateUniform<float>(n) {};
  void upload(){
    glUniform1f(location, value);
  }
};

template <>
class Uniform <GLuint> : public TemplateUniform <GLuint> {
public:
  Uniform(std::string n) : TemplateUniform<GLuint>(n) {};
  void upload(){
    glUniform1ui(location, value);
  }
};

template <>
class Uniform <GLint> : public TemplateUniform <GLint> {
public:
  Uniform(std::string n) : TemplateUniform<GLint>(n) {};
  void upload(){
    glUniform1i(location, value);
  }
};

/**
 * vectors
 */
template <>
class Uniform <glm::vec2> : public TemplateUniform <glm::vec2> {
public:
  Uniform(std::string n) : TemplateUniform<glm::vec2>(n) {};
  void upload(){
    glUniform2f(location, value.x, value.y);
  }
};

template <>
class Uniform <glm::vec3> : public TemplateUniform <glm::vec3> {
public:
  Uniform(std::string n) : TemplateUniform<glm::vec3>(n) {};
  void upload(){
    glUniform3f(location, value.x, value.y, value.z);
  }
};

template <>
class Uniform <glm::vec4> : public TemplateUniform <glm::vec4> {
public:
  Uniform(std::string n) : TemplateUniform<glm::vec4>(n) {};
  void upload(){
    glUniform4f(location, value.x, value.y, value.z, value.w);
  }
};

/**
 * Matrices
 */
template <>
class Uniform <glm::mat3> : public TemplateUniform <glm::mat3> {
public:
  Uniform(std::string n) : TemplateUniform<glm::mat3>(n) {};
  void upload(){
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
  }
};

template <>
class Uniform <glm::mat4> : public TemplateUniform <glm::mat4> {
public:
  Uniform(std::string n) : TemplateUniform<glm::mat4>(n) {};
  void upload(){
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
  }
};

/**
 * Arrays
 */
template <>
class Uniform <std::vector<float> > : public TemplateUniform <std::vector<float> > {
public:
  Uniform(std::string n) : TemplateUniform<std::vector<float> >(n) {};
  void upload(){
    glUniform1fv(location, value.size(), value.data());
  }
};

template <>
class Uniform <std::vector<glm::vec4> > : public TemplateUniform <std::vector<glm::vec4> > {
public:
  Uniform(std::string n) : TemplateUniform<std::vector<glm::vec4> >(n) {};
  void upload(){
    glUniform4fv(location, value.size(), reinterpret_cast<float*>(value.data()));
  }
};




template <>
class Uniform <Texture2D*> : public TemplateUniform <Texture2D*> {
public:
  Uniform(std::string n) : TemplateUniform<Texture2D*>(n) {};
  void upload(){
    (*value)(textureBucket);
    glUniform1i(location, textureLocation);
  }
  void setTextureLocation(GLint texLoc, GLenum texBucket){
    textureLocation = texLoc;
    textureBucket = texBucket;
  }
private:
  GLint textureLocation;
  GLenum textureBucket;
};
