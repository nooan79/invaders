#pragma once


enum class AlienScriptId
{
	alien = 0,
	count
};


class Game;
class PlayField;
struct GameConfig;

struct ScriptArgs
{
	float      dt;
	Game*      game;
	PlayField* world;
	const GameConfig* gameConfig;
};
