#pragma once

#include "Vector2D.h"
#include "Random.h"
#include <vector>
#include <random>
#include <memory>


class PlayerShip;
class Alien;
class PlayerLaser;
class AlienLaser;
class Explosion;
class PowerUp;
class Wall;
struct GameConfig;
struct RenderItem;
class MessageLog;
class VisualManager;
class CollisionSpace;


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

	void SpawnPlayerLaser(const PlayerLaser& laser);

	//
	void SpawnAlienLaser(const AlienLaser& laser);

	void AddPlayerShip(const PlayerShip& playerShip);

	void AddAlienShip(const Alien& alienShip);

	void AddExplosion(const Vector2D& position, float timer, float delay = 0.f);

	void SpawnPowerUp(const Vector2D& position, int type);

	void SpawnRandomPowerUp(const Vector2D& position);

	void SpawnBomb();

	void AddWall(const Vector2D& position);

	void EndGame();

	// TODO Remove gamePlay from PlayField
	void Update(float dt);

	void DestroyAll();

	void DestroyWalls();

	int GetAvailablePlayerLasers() const;

private:

	// Collision matrix
	void CheckCollisions();
	// Collision callbacks
	void Collision_LaserVSLaser(void* ud0, void* ud1);
	void Collision_AlienVSLaser(void* ud0, void* ud1);
	void Collision_PlayerVSLaser(void* ud0, void* ud1);
	void Collision_PlayerVSPowerUp(void* ud0, void* ud1);
	void Collision_PlayerVSAlien(void* ud0, void* ud1);
	void Collision_LaserVSWall(void* ud0, void* ud1);
	void Collision_AlienVSWall(void* ud0, void* ud1);

	void ActivatePowerUp(PlayerShip& player, const PowerUp& powerUp, MessageLog& messageLog);
	void AddScore(int score, int playerId);

private:

	std::default_random_engine& rGen;
	MessageLog&                 messageLog;
	const GameConfig&           config;

	// Game objects
	std::vector<PlayerShip>  players;

	std::vector<Alien>       aliens;
	std::vector<PlayerLaser> playerLasers;
	std::vector<AlienLaser>  alienLasers;
	std::vector<Explosion>   explosions;
	std::vector<PowerUp>     powerUps;
	std::vector<Wall>        walls;

	// Component managers
	std::unique_ptr<VisualManager> visualManager;
	std::unique_ptr<CollisionSpace> collisionSpace;

	Vector2D                 bounds;
	std::uniform_real_distribution<float> rndFloat01;
	Random rndPowerUp;
	// Number of available active laser slots for aliens and player
	int availableAlienLasers;
	int availablePlayerLasers;
};
