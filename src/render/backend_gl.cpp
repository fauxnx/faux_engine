#include <glad/glad.h>
#include "faux_engine/render/backend_gl.h"
#include "faux_engine/core.h"

#include "internals.h"

#include "easylogging++.h"


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

  NXCore.renderer_.backend_ = (Backend*)&NXCore.renderer_.backendGL_;
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
  NXCore.renderer_.backend_ = (Backend*)&NXCore.renderer_.backendVK_;
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

nx::Result nx::BackendGL::uploadShader(Shader* s) {

  deleteShader(s);

  for (Shader::Source& src : s->sources_) {

    switch (src.type_) {
    case Shader::Type::Vertex:   src.handle_ = glCreateShader(GL_VERTEX_SHADER);   break;
    case Shader::Type::Fragment: src.handle_ = glCreateShader(GL_FRAGMENT_SHADER); break;
    case Shader::Type::Geometry: src.handle_ = glCreateShader(GL_GEOMETRY_SHADER); break;
    }

    glShaderSource(src.handle_, 1, &src.source_, nullptr);
    glCompileShader(src.handle_);

    GLint compiled = GL_FALSE;
    glGetShaderiv(src.handle_, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
      s->compiled_ = false;
      GLint length = 0;
      glGetShaderiv(src.handle_, GL_INFO_LOG_LENGTH, &length);
      GLchar* log;
      log = (GLchar*)malloc(length * sizeof(GLchar));
      glGetShaderInfoLog(src.handle_, length, nullptr, log);
      glDeleteShader(src.handle_);
      LOG(ERROR) << "Failed to compile shader: " << log;
      free(log);
    }

    glAttachShader(s->handle_, src.handle_);
  }

  glLinkProgram(s->handle_);

  GLint linked = GL_FALSE;
  glGetProgramiv(s->handle_, GL_LINK_STATUS, &linked);
  if (linked == GL_FALSE) {
    GLint length = 0;
    glGetProgramiv(s->handle_, GL_INFO_LOG_LENGTH, &length);
    GLchar* log;
    log = (GLchar*)malloc(length * sizeof(GLchar));
    glGetProgramInfoLog(s->handle_, length, &length, &log[0]);
    glDeleteProgram(s->handle_);
    LOG(ERROR) << "Failed to link shader: " << log;
  }

  return nx::Result::Success;
}

nx::Result nx::BackendGL::deleteShader(Shader* s) {

  for (Shader::Source& src : s->sources_) {
    if (src.handle_ != -1) {
      glDeleteShader(src.handle_);
      src.handle_ = -1;
    }
  }
  if (s->handle_ != -1) {
    glDeleteProgram(s->handle_);
    s->handle_ = -1;
  }
  s->compiled_ = false;

  return nx::Result::Success;
}

nx::Result nx::BackendGL::clear(float r, float g, float b, float a = 1.0f) {

  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  return nx::Result::Success;
}
