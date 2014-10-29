#pragma once

#include <string>
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

namespace shaderUtils{
  std::string pathToShader(std::string filename){
    return "shaders/"+filename;
  }
}
