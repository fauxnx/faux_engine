#include "faux_engine/render/renderer.h"

nx::Renderer::Renderer() {
  currentBackend_ = nullptr;
}
nx::Renderer::~Renderer() {}

nx::Result nx::Renderer::init(nx::Backend::Type backend) {

	switch (backend) {
	case nx::Backend::Vulkan:	backendVK_.init(); break;
	case nx::Backend::OpenGL: backendGL_.init(); break;
	}

  return nx::Result::Success;
}

nx::Result nx::Renderer::shutdown() {

	backendGL_.shutdown();
	backendVK_.shutdown();
	
	return nx::Result::Success;
}