#include "faux_engine/render/shader.h"
#include "faux_engine/core.h"
#include <string>

nx::Shader::Shader() {
  
  handle_ = -1;
}

nx::Shader::~Shader() { }

nx::Result nx::Shader::load(const char* path) {

  std::string path_s = path;
  std::string format = path_s.substr(path_s.find_last_of(".") + 1);
  std::string src;

  if (format == "frag") {
    return NXCore.renderer_.backend_->createShader(&handle_, Type::Fragment, src.c_str());
  }
  else if (format == "vert") {
    return NXCore.renderer_.backend_->createShader(&handle_, Type::Vertex, src.c_str());
  }
  
  return nx::Result::Success;
}