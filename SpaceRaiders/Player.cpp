#include "Player.h"
#include "PlayField.h"
#include "Laser.h"
#include "Input.h"
#include "Images.h"
#include "Prefabs.h"
#include "Collision.h"
#include <random>
#include <algorithm>
#include <cassert>


PlayerShip::PlayerShip(const Vector2D& initialPos, const PlayerPrefab& prefab, int id, std::shared_ptr<Input> input_, std::default_random_engine& rGen) : 
	pos { initialPos },
	prefab { &prefab },
	prevPos { initialPos },
	id { id },
	fireTimer { 0.f },
	fireBoost { 1.f },
	state { State::normal },
	speedBoost { 1.f },
	score { 0 },
	input { std::move(input_) },
	rGen(&rGen),
	rndFloat01 { 0, 1, },
	powerUpTimer { 0.f },
	invulnerabilityTime { 0.f },
	accumTime { 0.f },
	size {	GetImageSize(prefab.imageId) }
{
	laserShots = 0;
	doubleFire = false;
	tripleFire = false;
}


void PlayerShip::Move(float dt, const Vector2D& worldBounds)
{
	// Update input state
	input->Update(dt);

	const float halfWidth = size.x / 2.f;

	prevPos = pos;
	// The original code did not handle the first and last column robustly
	if (input->Left())
	{
		pos.x -= prefab->velocity * speedBoost * dt;
		pos.x = std::max(halfWidth, pos.x);
	}
	else if (input->Right())
	{
		pos.x += prefab->velocity * speedBoost * dt;
		pos.x = std::min(pos.x, worldBounds.x - halfWidth);
	}

	// Boosts lasts a certain amount of time
	powerUpTimer -= dt;
	if (powerUpTimer < 0.f)
	{
		// Restore normal values
		powerUpTimer = 0.f;
		speedBoost = 1.f;
		fireBoost = 1.f;
		tripleFire = false;
		doubleFire = false;
	}

	accumTime += dt; // useful for time based effects

	invulnerabilityTime = std::max(0.f, invulnerabilityTime - dt);
	if (invulnerabilityTime > 0)
	{
		// Flicker player color to indicate invulnerability
		visual.color = std::sin(20.f * accumTime) > 0.f ? prefab->invulnColor : prefab->color;
	}
	else
	{
		visual.color = prefab->color;
	}
	visual.imageId = prefab->imageId;
}


void PlayerShip::ShootLasers(float dt, PlayField& world, float laserVelocity, float fireRate)
{
	if (fireTimer == 0.f)
	{
		fireTimer = fireRate * (1.f + rndFloat01(*rGen)) * fireBoost;
	}
	fireTimer -= dt;

	constexpr Visual laserVisual[3] =
	{
		{ ImageId::playerLaser, Color::lightBlueIntense },
		{ ImageId::playerLaserLeft, Color::lightBlueIntense },
		{ ImageId::playerLaserRight, Color::lightBlueIntense },
	};

	// Randomly shoot laser shots
	if (fireTimer < 0.f)
	{
		fireTimer = 0.f; // reset it
		const float l = laserVelocity;
		const Vector2D laserPos = { pos.x + prefab->laserOffset * ((laserShots % 2) ? -1.f : 1.f), pos.y - size.y }; // spawn in front
		if (tripleFire)
		{
			if (world.GetAvailablePlayerLasers() >= 3)
			{
				world.SpawnPlayerLaser( Laser { laserPos, {  0, -l }, laserVisual[0], id, ColliderId::playerLaser } );  // straight
				world.SpawnPlayerLaser( Laser { laserPos, { -l * 0.71f, -l * 0.71f }, laserVisual[1], id, ColliderId::playerLaser } );  // 45 left
				world.SpawnPlayerLaser( Laser { laserPos, { +l * 0.71f, -l * 0.71f }, laserVisual[2], id, ColliderId::playerLaser } );  // 45 right
			}
			else
			{
				// Try shooting later
			}
		}
		else if (doubleFire)
		{
			if (world.GetAvailablePlayerLasers() >= 2)
			{
				const Vector2D laserPos_l = { pos.x - prefab->laserOffset, pos.y - size.y }; // spawn in front
				const Vector2D laserPos_r = { pos.x + prefab->laserOffset, pos.y - size.y }; // spawn in front
				world.SpawnPlayerLaser( Laser { laserPos_l, { 0, -l }, laserVisual[0], id, ColliderId::playerLaser } );  // straight
				world.SpawnPlayerLaser( Laser { laserPos_r, { 0, -l }, laserVisual[0], id, ColliderId::playerLaser } );  // straight
			}
			else
			{
				// Try shooting later
			}
		}
		else // single fire
		{
			world.SpawnPlayerLaser( Laser { laserPos, { 0, -l }, laserVisual[0], id, ColliderId::playerLaser } );  // straight
		}
		++laserShots;
	}
}


void PlayerShip::Destroy()
{
	if (invulnerabilityTime == 0.f)
	{
		state = State::dead;
	}
}


const Vector2D& PlayerShip::GetPosition() const
{
	return pos;
}


Collider PlayerShip::GetCollisionArea()
{
	return { this, ColliderId::player, prevPos, pos, size, };
}


const Visual& PlayerShip::GetVisual() const
{
	return visual;
}


PlayerShip::State PlayerShip::GetState() const
{
	return state;
}


void PlayerShip::SetSpeedBoost(float value)
{
	assert(value > 0);
	speedBoost = value;
	powerUpTimer = 10.f;
}


void PlayerShip::SetFireBoost(float boost)
{
	assert(boost > 0.f);
	fireBoost = 1.f / boost; // it decrease interval between shots
	powerUpTimer = 10.f; // Note specified in the rules, I'm giving the fire boost a fixed amount of time too
}


void PlayerShip::SetDoubleFire()
{
	tripleFire = false;
	doubleFire = true;
	powerUpTimer = 10.f;
}


void PlayerShip::SetTripleFire()
{
	tripleFire = true;
	doubleFire = false;
	powerUpTimer = 10.f;
}


void PlayerShip::SetInvulnerable(float timer)
{
	invulnerabilityTime = timer;
}


void PlayerShip::AddScore(int increment)
{
	assert(increment > 0);
	score += increment;
}


int PlayerShip::GetScore() const 
{
	return score; 
}


int PlayerShip::GetId() const
{
	return id;
}
