#ifndef __NX_BACKEND_H__
#define __NX_BACKEND_H__

#include "faux_engine/types.h"
#include "faux_engine/render/shader.h"

namespace nx {

  class Backend {
  public:
    enum class Type {
      Vulkan,
      OpenGL
    };

    Backend();
    virtual ~Backend() = 0;
    virtual nx::Result init() = 0;
    virtual nx::Result shutdown() = 0;
    virtual nx::Result makeCurrent() = 0;

    virtual nx::Result uploadShader(Shader* s) = 0;
    virtual nx::Result deleteShader(Shader* s) = 0;
    virtual nx::Result clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) = 0;

    
    bool initialized_;
  };

}

#endif