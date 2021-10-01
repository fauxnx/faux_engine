#ifndef __NX_SHADER_H__
#define __NX_SHADER_H__

#include "faux_engine/types.h"

namespace nx {

  class Shader {
  public:

    enum Type {
      Fragment,
      Vertex
    };

    Shader();
    ~Shader();

    nx::Result load(const char* src);

    u32 handle_;
  };

}

#endif