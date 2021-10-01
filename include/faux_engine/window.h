#ifndef __NX_WINDOW_H__
#define __NX_WINDOW_H__

#include "faux_engine/types.h"
#include <string>

namespace nx {
  class Window {
  public:
    
    nx::Result init(u32 width, u32 height, char *name = nullptr);

    u32 width();
    u32 height();
    std::string name();

    void resize(u32 width, u32 height);

    Window();
    ~Window();

  private:

    u32 width_;
    u32 height_;
    std::string name_;
  };
}

#endif
