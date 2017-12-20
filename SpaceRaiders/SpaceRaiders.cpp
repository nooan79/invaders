#include <random>
#include <chrono>
#include <ctime>
#include <thread>
#include <cassert>
#include <iostream>

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
#include "inih-master/ini.h"


namespace
{
	
void ReadGameConfig(GameConfig& config, const char* iniFile);

}


int main()
{
	GameConfig gameConfig;
	// Read game config from ini file
	ReadGameConfig(gameConfig, "../../game.ini");

	DLL consoleDLL;
	Console console;
	ConsoleModule consoleModule;
	if (!InitConsole(console, consoleModule, "Win32Console.dll"))
	{
		return 0;
	}
	consoleModule.hideCursor(console.handle);

	ScriptModule scriptModule;
	if (!InitScriptModule(scriptModule, "Scripts.dll"))
	{
		return 0;
	}

	std::default_random_engine rGen;

	const IVector2D consoleSize { gameConfig.worldWidth, gameConfig.worldHeight };
	const Vector2D worldSize { (float)gameConfig.worldWidth, (float)gameConfig.worldHeight };
	Renderer mainRenderer { consoleSize, console, consoleModule };
	if (! mainRenderer.InitializeConsole(gameConfig.fontSize))
	{
		return 0;
	}
	MessageLog messageLog;
	PlayField world { worldSize, gameConfig, rGen, messageLog };
	Game game { world, gameConfig, rGen, messageLog, scriptModule };

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
	while (game.GetState() != GameStateId::quit)
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
				game.Update(fixedDeltaTime);
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
	std::cout << "avgElapsedTime: " << avgElapsedTime << std::endl;

	return 0;
}


namespace
{

#define PARSE_FLOAT(varName) \
	if (!strcmp(name, #varName)) \
	{ \
		config.##varName = (float)std::atof(value); \
	}

//char Prev_section[50];
int INIParser(void* user, const char* section, const char* name, const char* value)
{
	GameConfig& config = *(GameConfig*)user;
/*	if (strcmp(section, Prev_section)) {
        printf("... [%s]\n", section);
        strncpy(Prev_section, section, sizeof(Prev_section));
        Prev_section[sizeof(Prev_section) - 1] = '\0';
    }*/

	PARSE_FLOAT(playerFireRate);
	PARSE_FLOAT(playerLaserVelocity);
	PARSE_FLOAT(alienLaserVelocity);

#if INI_HANDLER_LINENO
    printf("... %s=%s;  line %d\n", name, value, lineno);
#else
    printf("... %s=%s;\n", name, value);
#endif

    return 1;//strcmp(name, "user")==0 && strcmp(value, "parse_error")==0 ? 0 : 1;
}
#undef PARSE_FLOAT


void ReadGameConfig(GameConfig& config, const char* iniFileName)
{
	int e = ini_parse(iniFileName, INIParser, &config);
}

}
