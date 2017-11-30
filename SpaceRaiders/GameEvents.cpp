// SpaceRaiders.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "GameEvents.h"
#include <cstdlib>
#include <cassert>


const AlienWave alienWaves[] =
{
	{ 4, 4.5, 16.f, 1.f, true, 0, 1, },
	{ 4, 4.5, 16.f, -1.f, false, 2, 3, },
	{ 8, 4.5, 16.f, 1.f, true, 4, 5, },
	{ 8, 4.5, 16.f, 1.f, true, 6, 7, },
	{ 8, 4.5, 16.f, 1.f, true, 0, 1, },
	{ 8, 4.5, 16.f, 1.f, false, 2, 3, }
};

const WallInfo walls[] =
{
	6, 40.f, 20.f,
	6, 100.f, 24.f,
	6, 60.f, 30.f,
};

const BossInfo bossInfo[] =
{
	{ 80, 10, 10/* boss id*/ }
};

const char* hudMessages[] =
{
	"Here they come!",
	"Destroy All Aliens!",
	"Save the Ikans!",
	"Don't let them reach the bottom!",
	"Keep moving!"
};
const int numHUDMessages = _countof(hudMessages);

const Event gameEvents[] =
{
	{ EventType::nextLevel, 0.f, nullptr },
	{ EventType::wait, 0.f, nullptr },
	{ EventType::spawnWave, 0.f, &alienWaves[0], },
	{ EventType::message, 0.f, hudMessages[0], },
	{ EventType::spawnWave, 12.f, &alienWaves[1], },
	{ EventType::spawnWalls, 12.f, &walls[0], },
	{ EventType::spawnWave, 24.f, &alienWaves[2], },
	{ EventType::spawnWalls, 24.f, &walls[1], },
	{ EventType::message, 24.f, hudMessages[1], },
	{ EventType::spawnWave, 36.f, &alienWaves[3], },
	{ EventType::spawnWave, 48.f, &alienWaves[4], },
	{ EventType::message, 48.f, hudMessages[2], },
	{ EventType::boss, 60.f, &bossInfo[0], },
};
const int numGameEvents = _countof(gameEvents);

