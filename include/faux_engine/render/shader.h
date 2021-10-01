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
      Compute,
    };

    struct Source {
      const char* source_;
      Type type_;
      u32 handle_;
    };

    Shader();
    ~Shader();

    nx::Result load(const char* src);

  private:
    u32 handle_;
    std::vector<Source> sources_;
  };

}

#endif