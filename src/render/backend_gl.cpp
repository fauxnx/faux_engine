#include "faux_engine/render/backend_gl.h"
#include "faux_engine/core.h"

#include <glad/glad.h>
#include <iostream>
#include "easylogging++.h"

#include "internals.h"

nx::BackendGL::BackendGL() { 

  initialized_ = false;
}

nx::BackendGL::~BackendGL() { }

nx::Result nx::BackendGL::init() {

  createWindow();

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LOG(FATAL) << "Failed to initialize GLAD";
    return nx::Result::Error;
  }

  NXCore.renderer_.currentBackend_ = (Backend*)&NXCore.renderer_.backendGL_;
  LOG(TRACE) << "Successfully initialized GLAD";
  return nx::Result::Success;
}

nx::Result nx::BackendGL::shutdown() {

  if (initialized_) {

    // cleanup

  }

  return nx::Result::Success;
}

nx::Result nx::BackendGL::makeCurrent() {

  if (!initialized_) {
    init();
  }
  else {
    createWindow();
  }
  NXCore.settings_.backend_ = nx::Backend::Type::OpenGL;
  NXCore.renderer_.currentBackend_ = (Backend*)&NXCore.renderer_.backendVK_;
  return nx::Result::Success;
}

nx::Result nx::BackendGL::createWindow() {

  NXInternal.destroyActiveWindow();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  NXInternal.windowGL_ = glfwCreateWindow(NXCore.window_.width(), NXCore.window_.height(), NXCore.window_.name().size() == 0 ? "faux_engine" : NXCore.window_.name().c_str(), nullptr, nullptr);
  NXInternal.currentWindow_ = NXInternal.windowGL_;
  glfwMakeContextCurrent(NXInternal.windowGL_);

  return nx::Result::Success;
}