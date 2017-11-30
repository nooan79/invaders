#pragma once

#include "Vector2D.h"
#include "RenderItem.h"
#include <random>
#include <memory>


class PlayField;
class Input;
struct GameConfig;
struct PlayerPrefab;
struct CollisionArea;


class PlayerShip
{
public:

	enum class State
	{
		normal,
		dead
	};

	PlayerShip(const Vector2D& initialPos, const PlayerPrefab& prefab, int id, std::shared_ptr<Input> input_, std::default_random_engine& rGen);

	const Vector2D& GetPosition() const;
	CollisionArea GetCollisionArea() const;
	const Visual& GetVisual() const;
	State GetState() const;
	void Move(float dt, const Vector2D& worldBounds);
	void ShootLasers(float dt, PlayField& world, float laserVelocity, float fireRate);
	void Destroy();
	void SetSpeedBoost(float boost);
	void SetFireBoost(float boost);
	void SetTripleFire(bool enabled);
	void SetInvulnerable(float timer);
	void AddScore(int increment);
	int GetScore() const;
	int GetId() const;

private:

	Vector2D   pos;
	const PlayerPrefab* prefab;
	Vector2D   prevPos;
	Visual     visual;
	Vector2D   size;
	int        id;
	float      fireTimer;
	float      fireBoost;
	float      speedBoost;
	float      boostTimer;
	int        score;
	State      state;
	bool       tripleFire;
	float      invulnerabilityTime;
	float      accumTime;
	int        laserShots;
	std::shared_ptr<Input> input;
	std::default_random_engine* rGen;
	std::uniform_real_distribution<float> rndFloat01;
};

