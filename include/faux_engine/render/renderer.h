#ifndef __NX_RENDERER_H__
#define __NX_RENDERER_H__

#include "faux_engine/render/backend_vk.h"
#include "faux_engine/render/backend_gl.h"

namespace nx {

  class Renderer {
  public:
    Renderer();
    ~Renderer();

    nx::Result init(nx::Backend::Type backend);
    nx::Result shutdown();

    Backend* currentBackend_;
    BackendVK backendVK_;
    BackendGL backendGL_;
  };

}

#endif