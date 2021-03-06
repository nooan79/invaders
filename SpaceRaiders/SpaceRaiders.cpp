#include <random>
#include <chrono>
#include <ctime>
#include <thread>
#include <cassert>
#include <iostream>
#include <algorithm>

#include "Base.h"
#include "Vector2D.h"
#include "GameConfig.h"
#include "Game.h"
#include "Console.h"
#include "Renderer.h"
#include "PlayField.h"
#include "Input.h"
#include "MessageLog.h"
#include "DLL.h"
#include "src/ScriptModule.h"
#include "GameStates.h"
#include "inih-master/ini.h"
// States
#include "StartMenu.h"
#include "PauseScreen.h"
#include "PlayGameState.h"
#include "GameOverState.h"
#include "IntroScreen.h"
#include "VictoryScreen.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


namespace
{
	
void ReadGameConfig(GameConfig& config, const char* iniFile);
void RegisterGameStates(Game& game);

}


const char DIR_SEPARATOR =
#if defined(_WIN32) || defined(WIN32)
    '\\';
#else
    '/';
#endif

void cutPath(char* path)
{
    // get '/' or '\\' depending on unix/mac or windows.
	int e = (int)strlen(path) - 1;
	while (e >= 0)
	{
		if (path[e] == DIR_SEPARATOR)
		{
			path[e] = 0;
			return;
		}
		e--;
	}
}

int main()
{
    // Get the path and the name
	char exePath[MAX_PATH];
	GetModuleFileNameA( GetModuleHandle(NULL), exePath, sizeof(exePath));
	cutPath(exePath);

	GameConfig gameConfig;
	// Read game config from ini file
	ReadGameConfig(gameConfig, "game.ini");

	Console console;
	if (!InitConsole(console))
	{
		return 0;
	}
	HideConsoleCursor(console.handle);

	ScriptModule scriptModule;
    char tmp[MAX_PATH];
	sprintf(tmp, "%s%cScripts.dll", exePath, DIR_SEPARATOR);
	if (!InitScriptModule(scriptModule, tmp))
	{
		return 0;
	}

	std::default_random_engine rGen;

	const IVector2D consoleSize { gameConfig.worldWidth, gameConfig.worldHeight };
	const Vector2D worldSize { (float)gameConfig.worldWidth, (float)gameConfig.worldHeight };
	Renderer mainRenderer { consoleSize, console };
	if (! mainRenderer.InitializeConsole(gameConfig.fontSize))
	{
		return 0;
	}
	MessageLog messageLog;
	PlayField world { worldSize, gameConfig, rGen, messageLog };
	Game game = NewGame(world, gameConfig, rGen, messageLog, scriptModule);
	RegisterGameStates(game);

	EnterGameState(game, (int)GameStateId::start);

	// Simulation and rendering happen at a fixed rate
	const int fixedFrameTime = 16; // [ms]
	const float fixedDeltaTime = (float)fixedFrameTime / 1000.f; // [s]
	const auto fixedFrameTime_ms = std::chrono::milliseconds(fixedFrameTime);
	const auto fixedFrameTime_sc = std::chrono::duration_cast<std::chrono::steady_clock::duration>(fixedFrameTime_ms);

	RenderItemList rl;
	std::chrono::steady_clock::duration accumTime { 0 };
	std::chrono::steady_clock::duration sleepTime { 0 };
	std::chrono::steady_clock::duration elapsedTimeHistory[256] = { };
	uint frameIndex = 0;
	auto t0 = std::chrono::steady_clock::now();
	while (game.stateId != (int)GameStateId::quit)
	{
		//ReloadDLL(consoleDLL);
		ReloadScriptModule(scriptModule);

		auto t1 = std::chrono::steady_clock::now();
		const auto elapsedTime = t1 - t0; // includes logic, rendering and sleeping
		t0 = t1;
		
		elapsedTimeHistory[frameIndex]  = elapsedTime;
		frameIndex = (frameIndex + 1) % _countof(elapsedTimeHistory);

		// Update logic and console at a fixed frame rate
		accumTime += elapsedTime;
		int maxIter = 2;
		while (accumTime > fixedFrameTime_sc)
		{ 
			accumTime -= fixedFrameTime_sc;
			if (maxIter > 0) // limit number of ticks in case accumTime is big (when debugging for example)
			{
				maxIter--;
				UpdateKeyStates();
				RunGameState(game, fixedDeltaTime);
				world.GetRenderItems(rl);
				mainRenderer.Update(rl, game, messageLog);
				messageLog.DeleteOldMessages(fixedDeltaTime, 3.f);
			}
		}

		// Adjust sleepTime so that elapsedTime converges towards the fixed frame time
		auto diff = fixedFrameTime_sc - elapsedTime;
		sleepTime += diff / 10;
		if (sleepTime.count() > 0)
		{
			std::this_thread::sleep_for(sleepTime);
		}
	}

	// Calculate the mean iteratively to avoid overflows
	double avgElapsedTime = 0.;
	double i = 1.;
	for (auto t : elapsedTimeHistory)
	{
		avgElapsedTime += (t.count() - avgElapsedTime) / i;
		++i;
	}
//	std::cout << "avgElapsedTime: " << avgElapsedTime << std::endl;

	return 0;
}


namespace
{

#define PARSE_INT(varName, minv, maxv) \
	if (!strcmp(name, #varName)) \
	{ \
		config.##varName = std::max(minv, std::min(maxv, std::atoi(value))); \
	}

#define PARSE_FLOAT(varName, minv, maxv) \
	do { \
		if (!strcmp(name, #varName)) \
		{ \
			config.##varName = std::max(minv, std::min(maxv, (float)std::atof(value))); \
		} \
	} while(0)

#define PARSE_BOOL(varName) \
	if (!strcmp(name, #varName)) \
	{ \
		config.##varName = strcmp(value, "true") ? false : true; \
	}

int INIParser(void* user, const char* section, const char* name, const char* value)
{
	GameConfig& config = *(GameConfig*)user;
	PARSE_INT(maxAlienLasers, 0, 100);
	PARSE_INT(maxPlayerLasers, 0, 100);
	PARSE_FLOAT(playerFireRate, 0.f, 1000.f);
	PARSE_FLOAT(playerLaserVelocity, 0.f, 1000.f);
	PARSE_BOOL(godMode);
	PARSE_FLOAT(alienSpeedMul, 0.f, 100.f);
	PARSE_FLOAT(alienLaserVelocity, 0.f, 1000.f);
	PARSE_FLOAT(alienTransformEnergy, 0.f, 1000.f);
	PARSE_FLOAT(alienDownVelocity, 0.f, 1000.f);
	PARSE_FLOAT(alienFireRate, 0.f, 1000.f);
	PARSE_FLOAT(betterAlienFireRate, 0.f, 1000.f);
	PARSE_FLOAT(explosionTimer, 0.f, 100.f);
	PARSE_FLOAT(powerUpVelocity, 0.f, 100.f);
	PARSE_INT(powerUpHits, 0, 100);
	PARSE_FLOAT(powerUpFireBoost, 1.f, 10.f);
	return 1;
}
#undef PARSE_FLOAT
#undef PARSE_INT
#undef PARSE_BOOL


void ReadGameConfig(GameConfig& config, const char* iniFileName)
{
	ini_parse(iniFileName, INIParser, &config);
}


void RegisterGameStates(Game& game)
{
	RegisterGameState(game, &startMenuData, StartMenu, DisplayStartMenu, EnterStartMenu);
	RegisterGameState(game, &introScreenData, IntroScreen, DisplayIntroScreen, EnterIntroScreen);
	RegisterGameState(game, &playGameStateData, PlayGame, DisplayPlayGame, EnterPlayGame);
	RegisterGameState(game, nullptr, PauseScreen, DisplayPauseScreen, nullptr);
	RegisterGameState(game, nullptr, GameOverMenu, DisplayGameOver, nullptr);
	RegisterGameState(game, nullptr, VictoryScreen, DisplayVictoryScreen, nullptr);
	RegisterGameState(game, nullptr, nullptr, nullptr, nullptr);
}

}
