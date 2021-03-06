#pragma once


struct Game;
class Renderer;
struct PlayGameStateData;

void EnterPlayGame(void* data, Game& game, int currentState);
int PlayGame(Game& game, void* data, float dt);
void DisplayPlayGame(Renderer& renderer, const void* data);
extern PlayGameStateData playGameStateData;
