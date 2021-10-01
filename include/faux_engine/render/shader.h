#ifndef __NX_SHADER_H__
#define __NX_SHADER_H__

#include "faux_engine/types.h"

#include <vector>

namespace nx {

  class Shader {
  public:

    enum class Type {
      Vertex,
      Fragment,
      Geometry,
    };

    struct Source {
      const char* source_;
      Type type_;
      u32 handle_;
    };

    Shader();
    ~Shader();

    nx::Result load(std::vector<const char*> paths);
    nx::Result reload();

    u32 handle_;
    std::vector<Source> sources_;
    bool compiled_;
  };

}

#endif