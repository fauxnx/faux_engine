#ifndef __NX_BACKEND_H__
#define __NX_BACKEND_H__

#include "faux_engine/types.h"

namespace nx {

  class Backend {
  public:
    enum Type {
      Vulkan,
      OpenGL
    };
    Backend();
    virtual ~Backend() = 0;
    virtual nx::Result init() = 0;
    virtual nx::Result shutdown() = 0;
    virtual nx::Result makeCurrent() = 0;
    
    bool initialized_;
  };

}

#endif