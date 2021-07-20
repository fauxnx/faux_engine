#ifndef __NX_INTERNALS_H__
#define __NX_INTERNALS_H__

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

#define NXInternal    nx::InternalState::Instance()

struct GameState;

namespace nx {

  class InternalState {
  public:
    static InternalState& Instance();
  
    void InternalStartup(void* data);
    void InternalUpdate(void* data);
    void InternalInput(void* data);
    void InternalMainLoop(void* data);
    void InternalShutdown(void* data);

    GLFWwindow* currentWindow_;
    GLFWwindow* windowVK_;
    GLFWwindow* windowGL_;

    void destroyActiveWindow();

    ~InternalState();
  private:
    friend class std::unique_ptr<InternalState>;
    InternalState();
  };

}


#endif
