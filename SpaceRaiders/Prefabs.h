// SpaceRaiders.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "RenderItem.h"


struct AlienPrefab
{
	Visual   visual;
	int      health;
	float    speed;
};

const AlienPrefab& GetAlienPrefab(int index);


struct PlayerPrefab
{
	Visual visual;
	float  velocity;
	float  laserOffset;
};

const PlayerPrefab& GetPlayerPrefab(int index);
