#pragma once
#include <string>
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

namespace shaderUtils{
  std::string absolutePathToShader(std::string filename){
    return std::string(TOSTRING(SHADER_PATH))+"/"+filename;
  }
}
