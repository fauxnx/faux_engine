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

  private:
    nx::Result createWindow();
  };

}

#endif