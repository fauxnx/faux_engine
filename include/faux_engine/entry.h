#ifndef __NX_ENTRY_H__
#define __NX_ENTRY_H__

void NX_EntryPoint();

struct GameState;

void Startup(GameState* state);
void Input(GameState* state);
void Update(GameState* state, float delta_time);
void Render(GameState* state);
void Shutdown(GameState* state);

#endif
