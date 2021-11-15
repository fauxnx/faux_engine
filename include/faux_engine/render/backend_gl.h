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

    nx::Result uploadShader(Shader* s) override;
    nx::Result deleteShader(Shader* s) override;
    nx::Result clear(float r, float g, float b, float a) override;

    nx::Result uploadGeometry(Geometry* g) override;
    nx::Result drawGeometry(Geometry* g, bool use_material = true) override;

  private:
    nx::Result createWindow();
  };

}

#endif