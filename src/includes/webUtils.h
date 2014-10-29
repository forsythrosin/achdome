#pragma once

#include <string>
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

namespace webUtils{
  std::string pathToResource(std::string filename){
    return std::string("webResources/")+filename;
  }
}
