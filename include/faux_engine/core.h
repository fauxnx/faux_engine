#ifndef __NX_CORE_H__
#define __NX_CORE_H__


#include "faux_engine/render/renderer.h"
#include "faux_engine/window.h"
#include <memory>

#define NXCore    nx::Core::Instance()

namespace nx {

  class Core {
  public:

    struct Settings {
      u32 width_;
      u32 height_;
      void* data_;
      char* name_;
      nx::Backend::Type backend_;
    } settings_;

    static Core& Instance();

    nx::Result init(Settings s);
    Renderer renderer_;
    Window window_;

    ~Core();
  private:
    friend class std::unique_ptr<Core>;
    Core();
  };

}

#endif
