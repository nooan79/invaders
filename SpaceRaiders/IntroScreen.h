#pragma once


struct Game;
class Renderer;


// FIXME Add constructor
void EnterIntroScreen(void* data, Game& game, int currentState);
int IntroScreen(Game& game, void* data, float dt);
void DisplayIntroScreen(Renderer& renderer, const void* data);

struct IntroScreenData;
extern IntroScreenData introScreenData;