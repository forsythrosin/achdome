#pragma once
#include <string>
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

namespace webUtils{
  std::string absolutePathToResource(std::string filename){
    return std::string(TOSTRING(WEBRESOURCES_PATH))+"/"+filename;
  }
}
