#include "faux_engine/render/shader.h"
#include "faux_engine/core.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "easylogging++.h"

nx::Shader::Shader() {
  
  handle_ = -1;
}

nx::Shader::~Shader() { }

nx::Result nx::Shader::load(std::vector<const char*> paths) {

  sources_.clear();

  // Read and prepare all the sources
  for (const char* path : paths) {
    std::string path_s = path;

    Source s;

    std::string src_code;
    std::ifstream src_file;

    src_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
      src_file.open(path);
      std::stringstream src_stream;
      src_stream << src_file.rdbuf();

      src_file.close();
      src_code = src_stream.str();
    }
    catch (std::ifstream::failure& e) {
      LOG(ERROR) << "Error reading shader source: " << e.what();
      return nx::Result::Error;
    }

    s.source_ = src_code.c_str();

    std::string format = path_s.substr(path_s.find_last_of(".") + 1);

    if (format == "frag") {
      s.type_ = Type::Fragment;
    }
    else if (format == "vert") {
      s.type_ = Type::Vertex;
    }

    sources_.push_back(s);
  }

  return reload();
}

nx::Result nx::Shader::reload() {

  NXCore.renderer_.backend_->uploadShader(this);

  return nx::Result::Success;
}