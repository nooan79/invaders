#include "stdafx.h"
#include "Alien.h"
#include "Prefabs.h"
#include "PlayField.h"
#include "AlienLaser.h"
#include "GameConfig.h"
#include "Collision.h"
#include "Images.h"
#include <random>
#include <algorithm>
#include <cassert>


Alien::Alien(const Vector2D& initialPos, const Vector2D& velocity, std::default_random_engine& rGen, const AlienPrefab& normalPrefab, 
	const AlienPrefab& betterPrefab) :
	pos { initialPos },
	normalPrefab { &normalPrefab },
	betterPrefab { &betterPrefab },
	prevPos { initialPos },
	rndFloat { 0, 1 },
	rGen { &rGen },
	health { normalPrefab.health },
	velocity { velocity },
	fireTimer { 0.f },
	state { State::normal }
{
	// Set default
	energy = 0.f;
}


Visual Alien::GetVisual() const
{
	return (state == State::better) ? betterPrefab->visual : normalPrefab->visual;
}


const Vector2D& Alien::GetPosition() const
{
	return pos;
}


CollisionArea Alien::GetCollisionArea() const
{
	return { prevPos, pos, GetSize() };
}


Alien::State Alien::GetState() const
{
	return state;
}


void Alien::Move(float dt, const Vector2D& worldBounds)
{
	const Vector2D size = GetSize();
	const float halfWidth = size.x * 0.5f;
	prevPos = pos;
	pos = Mad(pos, velocity, dt);

	// Border check
	if (pos.x < halfWidth)
	{
		pos.x = halfWidth;
		pos.y += size.y;
		velocity.x = std::abs(velocity.x);
	}
	else if (pos.x > worldBounds.x - halfWidth)
	{
		pos.x = (float)worldBounds.x - halfWidth;
		velocity.x = -std::abs(velocity.x);
		pos.y += size.y;
	}
}


void Alien::Update(float dt, PlayField& world, const GameConfig& gameConfig)
{
	const Vector2D size = GetSize();
	// Border check vertical:
	if (pos.y >= world.GetBounds().y - size.y)
	{
		// If an alien ship reaches the bottom of the screen the players die
		world.EndGame();
	}
	if (pos.y > world.GetBounds().y - size.y * 0.5f)
	{
		Destroy();  // destroy the ship
	}

	// The amount of energy increases randomly per frame
	energy += rndFloat(*rGen) * gameConfig.alienUpdateRate;

	// State machine
	switch (state)
	{
		case State::normal:
			if (energy >= gameConfig.alienTransformEnergy)
			{
				// The alien ship can transform into a better Alien. The actual transformation happens randomly.
				state = State::evolving;
			}
			break;
		case State::evolving:
			if (rndFloat(*rGen) < gameConfig.alienTransformRate)
			{
				// Transform into a better alien
				Transform();
			}
			break;
		default:
			break;
	};

	if (fireTimer == 0.f)
	{ 
		SetFireTimer(gameConfig);
	}
	fireTimer -= dt;
	// Randomly shoot lasers
	if (fireTimer < 0.f)
	{
		const Vector2D laserPos = { pos.x, pos.y + size.y * 0.5f }; // spawn in front
		world.SpawnAlienLaser( AlienLaser { laserPos, gameConfig.alienLaserVelocity } );
		fireTimer = 0.f; // reset it
	}
}


void Alien::Destroy()
{
	state = State::dead;
}


void Alien::DecreaseHealth() 
{ 
	health -= 1; 
	if (health <= 0)
	{
		state = State::dead;
	}
}


void Alien::AvoidWall(const Vector2D& wallPos)
{
	const Vector2D size = GetSize();
	// Reflect trajectory against wall (quad shape)
	const Vector2D wallNormal = ComputeClosestNormal(velocity);
	Vector2D vr = Reflect(velocity, wallNormal);
	vr = Normalize(vr);
	Vector2D closestWallPoint = Mad(wallPos, wallNormal, 2.f); 
	Vector2D collisionPoint = closestWallPoint;
	// Push alien position a safe distance away from the wall so that it doesn't get stuck in a collision loop
	prevPos = pos;
	pos = Mad(collisionPoint, vr, size.x * 1.5f); // FIXME
	// Update velocity
	velocity = Mul(vr, Length(velocity));
	velocity.y = std::max(0.f, velocity.y); // don't go upwards
}


void Alien::Transform()
{
	state = State::better;
	const float sign = velocity.x > 0.f ? -1.f : +1.f;
	velocity.x = betterPrefab->speed * sign;
	health = betterPrefab->health;
}


void Alien::SetFireTimer(const GameConfig& gameConfig)
{
	fireTimer = state == State::normal ? gameConfig.alienFireRate : gameConfig.betterAlienFireRate;
	// Randomize it
	fireTimer *= (1.f + rndFloat(*rGen));
}


Vector2D Alien::GetSize() const
{
	const Image* image = GetImage(state == State::normal ? normalPrefab->visual.imageId : betterPrefab->visual.imageId);
	return { (float)image->width, (float)image->height };
}
