#include "sgct.h"

class AbstractUniform {
public:
  AbstractUniform(std::string n) : name(n), location(-1) {};

  virtual void upload() = 0;
  std::string getName() { return name; };
  void setLocation(GLuint l) { location = l; };

  bool needsUpdate = false;

protected:
  std::string name;
  GLuint location;
};

template <typename T>
class TemplateUniform : public AbstractUniform {
public:
  TemplateUniform(std::string n) : AbstractUniform(n), value(T()) {};
  void set(T t) { value = t; };

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
