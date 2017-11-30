// SpaceRaiders.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"


enum class EventType
{
	nextLevel,
	wait,
	message,
	spawnWave,
	spawnWalls,
	boss,
};


struct Event
{
	EventType   type;
	float       time;
	const void* data;
};
