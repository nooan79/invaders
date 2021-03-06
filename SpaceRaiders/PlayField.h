#pragma once

#include "Vector2D.h"
#include "Random.h"
#include <vector>
#include <random>
#include <memory>


struct PlayerShip;
struct Alien;
struct Laser;
struct Explosion;
struct PowerUp;
struct Wall;
struct GameConfig;
struct RenderItem;
class MessageLog;
class CollisionSpace;
struct ScriptModule;


class PlayField
{
public:

	PlayField(const Vector2D& iBounds, const GameConfig& config, std::default_random_engine& rGen, MessageLog& messageLog);
	~PlayField();

	const std::vector<PlayerShip>& GetPlayers() const;
	bool NoAliens() const;

	void GetRenderItems(std::vector<RenderItem>& ritems);
	const Vector2D& GetBounds() const;

	void Restart();

	void SpawnPlayerLaser(const Laser& laser);

	//
	void SpawnAlienLaser(const Laser& laser);

	void AddPlayerShip(const PlayerShip& playerShip);

	void AddAlienShip(const Alien& alienShip);

	void AddExplosion(const Vector2D& position, float timer, float delay = 0.f);

	void SpawnPowerUp(const Vector2D& position, int type);

	void SpawnRandomPowerUp(const Vector2D& position);

	void SpawnBomb();

	void AddWall(const Vector2D& position);

	void DeletePlayers();

	// TODO Remove gamePlay from PlayField
	void Update(float dt, const ScriptModule& scriptModule);

	void DestroyAll();

	void DestroyWalls();

	int GetAvailablePlayerLasers() const;

public:

	void AddScore(int score, int playerId);

	std::default_random_engine& rGen;
	MessageLog&                 messageLog;
	const GameConfig&           config;

	// Game objects
	std::vector<PlayerShip>  players;
	std::vector<Alien>       aliens;
	std::vector<Laser>       lasers;
	std::vector<Explosion>   explosions;
	std::vector<PowerUp>     powerUps;
	std::vector<Wall>        walls;

	Vector2D                 bounds;
	std::uniform_real_distribution<float> rndFloat01;
	Random rndPowerUp;
	// Number of available active laser slots for aliens and player
	int availableAlienLasers;
	int availablePlayerLasers;
};
