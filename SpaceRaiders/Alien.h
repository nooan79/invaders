#pragma once

#include "Vector2D.h"
#include "RenderItem.h"
#include <random>


class PlayField;
struct GameConfig;
struct AlienPrefab;
struct CollisionArea;


class Alien
{
public:

	enum class State
	{
		normal,
		evolving,
		better,
		dead
	};

	Alien(const Vector2D& initialPos, const Vector2D& velocity, std::default_random_engine& rGen, const AlienPrefab& normalPrefab, 
		const AlienPrefab& betterPrefab);

	Visual GetVisual() const;
	const Vector2D& GetPosition() const;
	CollisionArea GetCollisionArea() const;
	State GetState() const;
	void Move(float dt, const Vector2D& worldBounds);
	void Update(float dt, PlayField& world, const GameConfig& config);
	void Destroy();
	void DecreaseHealth();
	void AvoidWall(const Vector2D& wallPos);

private:

	void SetFireTimer(const GameConfig& gameConfig);
	void Transform();
	Vector2D GetSize() const;

private:

	Vector2D   pos;
	const AlienPrefab* normalPrefab;
	const AlienPrefab* betterPrefab;
	Vector2D   prevPos;
	State      state;
	std::uniform_real_distribution<float> rndFloat;
	std::default_random_engine* rGen;
	Vector2D velocity;
	int health;
	float energy;
	float fireTimer;
};
