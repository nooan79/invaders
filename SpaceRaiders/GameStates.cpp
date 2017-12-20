#include "GameStates.h"
#include "Game.h"
#include "PlayField.h"
#include "Player.h"
#include "Input.h"
#include "MessageLog.h"
#include "GameEvents.h"
#include "Renderer.h"
#include <cassert>


namespace
{
// Images
const wchar_t ikaLogo[] =
{
	#include "IKA.txt"
};
const wchar_t raidersTxt[] =
{
	#include "raiders.txt"
};
const Image ikaImg = { ikaLogo, 25, 6 };
const Image raidersImg = { raidersTxt, 46, 6 };

GameStateId StartMenu(Game& game, float dt);
GameStateId Intro(Game& game, float dt);
GameStateId VictoryMenu(Game& game, float dt);
GameStateId PauseMenu(Game& game, float dt);
GameStateId GameOverMenu(Game& game, float dt);
GameStateId Run(Game& game, float dt);
void DrawStartMenu(Renderer& renderer);
void DisplayIntro(Renderer& renderer);
void DisplayPauseMenu(Renderer& renderer);
void DisplayVictory(Renderer& renderer);
void DisplayGameOver(Renderer& renderer);

struct GameState
{
	// FIXME Enter and Exit function ? Start for example should destroy all objects
	using RunFunc = GameStateId (*)(Game& game, float dt);
	using DrawFunc = void (*)(Renderer& renderer);

	RunFunc  runFunc;
	DrawFunc drawFunc;
};

const GameState gameStates[(int)GameStateId::count] =
{
	{ StartMenu, DrawStartMenu },
	{ Intro, DisplayIntro },
	{ Run, nullptr },
	{ PauseMenu, DisplayPauseMenu },
	{ GameOverMenu, DisplayGameOver },
	{ VictoryMenu, DisplayVictory },
	{ nullptr, nullptr }
};

}


GameStateId RunGameState(GameStateId id, Game& game, float dt)
{
	assert((int)id < _countof(gameStates));
	auto fnc = gameStates[(int)id].runFunc;
	return fnc ? fnc(game, dt) : id;
}

void DrawGameState(GameStateId id, Renderer& r)
{
	assert((int)id < _countof(gameStates));
	auto fnc = gameStates[(int)id].drawFunc;
	if (fnc)
	{
		fnc(r);
	}
}


namespace
{

GameStateId StartMenu(Game& game, float /*dt*/)
{
	GameStateId newState;
	game.numPlayers = 0;
	game.messageLog.Clear();
	game.world.DestroyAll(); // FIXME OnEnter

	if (KeyJustPressed(KeyCode::_1))
	{
		game.Start(Game::Mode::p1);
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::_2))
	{
		game.Start(Game::Mode::cpu1);
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::_3))
	{
		game.Start(Game::Mode::p1p2);
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::_4))
	{
		game.Start(Game::Mode::p1cpu2);
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::_5))
	{
		game.Start(Game::Mode::cpu1cpu2);
		newState = GameStateId::intro;
	}
	else if (KeyJustPressed(KeyCode::escape))
	{
		newState = GameStateId::quit;
	}
	else
	{
		newState = GameStateId::start;
	}
	return newState;
}


GameStateId Intro(Game& game, float dt)
{
	GameStateId nextState = GameStateId::intro;
	game.accumTime += dt;
	if (AnyKeyJustPressed() || game.accumTime > 8.f)
	{
		game.SetLevel(0);
		nextState = GameStateId::running;
	}
	return nextState;
}


GameStateId VictoryMenu(Game& game, float dt)
{
	GameStateId newState = GameStateId::victory;
	if (AnyKeyJustPressed())
	{
		newState = GameStateId::start;
	}
	return newState;
}


GameStateId PauseMenu(Game& game, float dt)
{
	GameStateId newState = GameStateId::paused;
	if (KeyJustPressed(KeyCode::escape))
	{
		newState = GameStateId::start;
	}
	else if (KeyJustPressed(KeyCode::enter))
	{
		newState = GameStateId::running;
	}
	return newState;
}


GameStateId GameOverMenu(Game& game, float dt)
{
	GameStateId newState = GameStateId::over;
	if (AnyKeyJustPressed())
	{
		newState = GameStateId::start;
	}
	return newState;
}


GameStateId Run(Game& game, float dt)
{
	GameStateId newState = GameStateId::running;
	PlayField& world = game.world;
	if (world.GetPlayers().empty())
	{
		game.messageLog.Clear();
		newState = GameStateId::over;
		return newState;
	}

	// Update score
	for (const auto& player : world.GetPlayers())
	{
		assert(player.GetId() < game.maxPlayers);
		game.score[player.GetId()] = player.GetScore();
	}

	const Level& level = GetLevel(game.levelIndex);
	if (game.eventIndex >= level.numEvents && world.NoAliens())
	{
		if (game.levelIndex < GetNumLevels() - 1)
		{
			game.NextLevel();
		}
		else
		{
			newState = GameStateId::victory;
		}
		return newState;
	}

	game.accumTime += dt;
	if (game.eventIndex < level.numEvents && game.accumTime > level.events[game.eventIndex].time)
	{
		game.ProcessEvent(level.events[game.eventIndex]);
		++game.eventIndex;
	}

	if (KeyJustPressed(KeyCode::escape))
	{
		newState = GameStateId::paused;
	}
	world.Update(dt, game.scriptModule);

	return newState;
}


void DrawStartMenu(Renderer& renderer)
{
	renderer.DrawImage(ikaImg, 0, 0, Color::greenIntense, Renderer::Alignment::centered, Renderer::Alignment::top);
	renderer.DrawImage(raidersImg, 0, 7, Color::greenIntense, Renderer::Alignment::centered,  Renderer::Alignment::top);

	static const char* str[] =
	{
		"",
		"Single Player",
		"  Press 1 to start one player game",
		"  Press 2 to start one CPU game",
		"",
		"Multi Player",
		"  Press 3 to start two players game",
		"  Press 4 to start player and CPU game",
		"  Press 5 to start two CPUs game",
		"",
		"Press ESC to quit"
	};
	constexpr int numRows = _countof(str);
	const int row = 16;
	const int col = (renderer.GetBounds().x - (int)strlen(str[3])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.ClearLine(row + r);
		renderer.DisplayText(str[r], col, row + r, Color::white);
	}
}


void DisplayIntro(Renderer& renderer)
{
	renderer.DrawImage(GetImage(ImageId::planet), 0, 8, Color::white, Renderer::Alignment::centered, Renderer::Alignment::top);
	static const char* str[] =
	{
		"",
		"Planet IKA is under attack!",
		"Destroy the alien invaders before they reach the ground",
		"Save the planet and earn eternal glory",
		""
	};
	constexpr int numRows = _countof(str);
	const IVector2D& bounds = renderer.GetBounds();
	const int row = (bounds.y - numRows) / 2; // centered
	const int col = (bounds.x - (int)strlen(str[2])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.ClearLine(row + r);
		renderer.DisplayText(str[r], col, row + r, r == 0 ? Color::greenIntense : Color::white);
	}
}


void DisplayPauseMenu(Renderer& renderer)
{
	static const char* str[] =
	{
		"",
		"Press ENTER to continue game",
		"Press ESC to go back to main menu",
		""
	};
	constexpr int numRows = _countof(str);
	const IVector2D& bounds = renderer.GetBounds();
	const int row = (bounds.y - numRows) / 2; // centered
	const int col = (bounds.x - (int)strlen(str[1])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.ClearLine(row + r);
		renderer.DisplayText(str[r], col, row + r, Color::white);
	}
}


void DisplayVictory(Renderer& renderer)
{
	static const char* str[] =
	{
		"",
		"You saved the planet!",
		"",
		"Press any key to continue",
		""
	};
	constexpr int numRows = _countof(str);
	const int row = (renderer.bounds.y - numRows) / 2; // centered
	const int col = (renderer.bounds.x - (int)strlen(str[3])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.ClearLine(row + r);
		renderer.DisplayText(str[r], col, row + r, Color::white);
	}
}


void DisplayGameOver(Renderer& renderer)
{
	static const char* str[] =
	{
		"",
		"Game Over",
		"",
		"Press any key to continue",
		""
	};
	constexpr int numRows = _countof(str);
	const int row = (renderer.bounds.y - numRows) / 2; // centered
	const int col = (renderer.bounds.x - (int)strlen(str[3])) / 2;
	for (int r = 0; r < numRows; ++r)
	{
		renderer.ClearLine(row + r);
		renderer.DisplayText(str[r], col, row + r, Color::white);
	}
}


}
