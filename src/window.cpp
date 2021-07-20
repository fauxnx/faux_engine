#include "faux_engine/types.h"
#include "faux_engine/window.h"
#include "internals.h"
#include "easylogging++.h"

#include <memory>


nx::Window::Window() {
  width_ = 0;
  height_ = 0;
}

nx::Window::~Window() {
  glfwTerminate();
}

nx::Result nx::Window::init(u32 width, u32 height, char *name) {
  width_ = width;
  height_ = height;
  name_ = name == nullptr ? "" : name;

  if (glfwInit() == GLFW_FALSE) {
    LOG(ERROR) << "Failed to initialize window";
    return nx::Result::Error;
  }

  LOG(TRACE) << "Successfully initialized window";
  return nx::Result::Success;
}

u32 nx::Window::width() {
  return width_;
}

u32 nx::Window::height() {
  return height_;
}

std::string nx::Window::name() {
  return name_;
}

void nx::Window::resize(u32 width, u32 height) {
  width_ = width;
  height_ = height;
}

bool nx::Window::shouldClose() {
  return glfwWindowShouldClose(NXInternal.currentWindow_);
}

void nx::Window::pollEvents() {
  glfwPollEvents();
}