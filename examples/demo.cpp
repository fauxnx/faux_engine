#include <faux_engine/entry.h>
#include <faux_engine/core.h>
#include <faux_engine/window.h>
#include <faux_engine/types.h>
#include <memory>

struct GameState {

};

void Startup(GameState* state) {
  nx::Geometry g;
  g.loadFromObj("../data/cube.obj");
  g.upload();
}

void Input(GameState* state) {

}

void Update(GameState* state, float delta_time) {
  NXCore.renderer_.backend_->clear();

}

void Render(GameState* state) {

}

void Shutdown(GameState* state) {

}

void NX_EntryPoint() {
  std::unique_ptr<GameState> state;
  nx::Core::Settings s;
  s.width_ = 1280;
  s.height_ = 800;
  s.data_ = state.get();
  s.name_ = "Demo";
  s.backend_ = nx::Backend::Type::OpenGL;

  NXCore.init(s);
}
