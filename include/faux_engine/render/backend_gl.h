#ifndef __NX_BACKEND_GL_H__
#define __NX_BACKEND_GL_H__

#include "faux_engine/render/backend.h"

namespace nx {

  class BackendGL : Backend {
  public:
    BackendGL();
    ~BackendGL();

    nx::Result init() override;
    nx::Result shutdown() override;
    nx::Result makeCurrent() override;

    nx::Result createShader(u32* handle, Shader::Type type, const char* src) override;
    nx::Result clear(float r, float g, float b, float a) override;

  private:
    nx::Result createWindow();
  };

}

#endif