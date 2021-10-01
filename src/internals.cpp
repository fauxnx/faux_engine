#include "faux_engine/entry.h"
#include "faux_engine/types.h"
#include "faux_engine/window.h"
#include "faux_engine/core.h"
#include "internals.h"

nx::InternalState::InternalState() {
  currentWindow_ = nullptr;
  windowVK_ = nullptr;
  windowGL_ = nullptr;
}
nx::InternalState::~InternalState() {}

nx::InternalState& nx::InternalState::Instance() {
  static std::unique_ptr<InternalState> internal_state;
  if (nullptr == internal_state.get()) internal_state = std::unique_ptr<InternalState>(new InternalState());
  return *internal_state.get();
}

void nx::InternalState::InternalStartup(void* data) {
  GameState* state = reinterpret_cast<GameState*>(data);
  Startup(state);

  InternalMainLoop(data);
}

void nx::InternalState::InternalUpdate(void* data) {
  GameState* state = reinterpret_cast<GameState*>(data);
  Update(state, 0.016f);
}

void nx::InternalState::InternalInput(void* data) {
  GameState* state = reinterpret_cast<GameState*>(data);
  Input(state);
}

void nx::InternalState::InternalShutdown(void* data) {
  GameState* state = reinterpret_cast<GameState*>(data);

  NXCore.renderer_.shutdown();

  Shutdown(state);
}

void nx::InternalState::InternalMainLoop(void* data) {

  while (!glfwWindowShouldClose(currentWindow_)) {
    glfwPollEvents();

    InternalUpdate(data);
    glfwSwapBuffers(currentWindow_);
  }
  InternalShutdown(data);
}

void nx::InternalState::destroyActiveWindow() {

  glfwDestroyWindow(currentWindow_);
  glfwMakeContextCurrent(nullptr);
}
