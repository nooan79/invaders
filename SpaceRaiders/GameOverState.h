#pragma once


struct Game;
class Renderer;


int GameOverMenu(Game& game, void* data, float dt);
void DisplayGameOver(Renderer& renderer, const void* gameState);
