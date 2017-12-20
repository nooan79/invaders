#include "PlayField.h"
#include "Player.h"
#include "Laser.h"
#include "Alien.h"
#include "Explosion.h"
#include "PowerUp.h"
#include "Wall.h"
#include "GameConfig.h"
#include "Renderer.h"
#include "Utils.h"
#include "MessageLog.h"
#include "Images.h"
#include "CollisionSpace.h"
#include <algorithm>
#include <cassert>
#include <functional>


PlayField::PlayField(const Vector2D& iBounds, const GameConfig& config, std::default_random_engine& rGen, MessageLog& messageLog) : 
	bounds { iBounds },
	config { config },
	rGen { rGen },
	messageLog { messageLog },
	rndFloat01 { 0.f, 1.f },
	rndPowerUp { (int)PowerUp::count, rGen },
	collisionSpace { std::make_unique<CollisionSpace>() }
{}


PlayField::~PlayField() = default;


const std::vector<PlayerShip>& PlayField::GetPlayers() const
{
	return players;
}


bool PlayField::NoAliens() const
{
	return aliens.empty();
}


void PlayField::Restart()
{
	availableAlienLasers = config.maxAlienLasers;
	availablePlayerLasers = config.maxPlayerLasers * (int)players.size();
	rndPowerUp.Reset();
}


void PlayField::SpawnPlayerLaser(const Laser& laser)
{
	if (availablePlayerLasers > 0)
	{
		availablePlayerLasers--;
		lasers.push_back(laser);
	}
}

//
void PlayField::SpawnAlienLaser(const Laser& laser)
{
	if (availableAlienLasers > 0)
	{
		availableAlienLasers--;
		lasers.push_back(laser);
	}
}

void PlayField::AddPlayerShip(const PlayerShip& playerShip)
{
	players.push_back(std::move(playerShip));
}

void PlayField::AddAlienShip(const Alien& alienShip)
{
	aliens.push_back(alienShip);
}

void PlayField::AddExplosion(const Vector2D& position, float timer, float delay)
{
	explosions.emplace_back(position, timer, delay);
}


void PlayField::SpawnRandomPowerUp(const Vector2D& position)
{
	// Randomly choose power up
	const int type = rndPowerUp.Next();
	SpawnPowerUp(position, type);
}


void PlayField::SpawnPowerUp(const Vector2D& position, int type)
{
	constexpr int c = (int)PowerUp::count;
	constexpr Visual visuals[c] =
	{
		{ ImageId::sPowerUp, Color::yellowIntense, },
		{ ImageId::fPowerUp, Color::yellowIntense, },
		{ ImageId::dPowerUp, Color::yellowIntense, },
		{ ImageId::tPowerUp, Color::yellowIntense, },
		{ ImageId::iPowerUp, Color::yellowIntense, },
		{ ImageId::bomb,     Color::yellowIntense, },
	};
	powerUps.push_back( PowerUp { position, visuals[type], { 5.f, 3.f }, config.powerUpVelocity, (PowerUp::Type)type } );
}


void PlayField::SpawnBomb()
{
	for (int i = 0; i < 60; ++i)
	{
		float x = 10.f + (bounds.x - 20.f) * rndFloat01(rGen);
		float y = 10.f + (bounds.y - 20.f) * rndFloat01(rGen);
		float delay = (float)i * .05f;
		AddExplosion( { x, y }, config.explosionTimer, delay);
	}
	// All aliens take damage
	for (auto& alien : aliens)
	{
		Alien_DecreaseHealth(alien);
	}
}


void PlayField::AddWall(const Vector2D& position)
{
	walls.emplace_back(position, config.wallMaxHits);
}


void PlayField::DeletePlayers()
{
	players.clear();
}
	

void PlayField::GetRenderItems(std::vector<RenderItem>& ritems)
{
	ritems.reserve(players.size() + aliens.size() + lasers.size() + explosions.size() );
	ritems.clear();

	for (const auto& wall : walls)
	{
		ritems.push_back( { wall.GetPosition(), wall.GetVisual() } );
	}
	for (const auto& player : players)
	{
		ritems.push_back( { player.GetPosition(), player.GetVisual() } );
	}
	for (const auto& alienShip : aliens)
	{
		ritems.push_back( Alien_GetRenderItem(alienShip));
	}
	for (const auto& laser : lasers)
	{
		ritems.push_back( { laser.GetPosition(), laser.GetVisual() } );
	}
	for (const auto& explosion : explosions)
	{
		ritems.push_back( { explosion.GetPosition(), explosion.GetVisual() } );
	}
	for (const auto& powerUp : powerUps)
	{
		ritems.push_back( { powerUp.GetPosition(), powerUp.GetVisual() } );
	}
}


const Vector2D& PlayField::GetBounds() const
{
	return bounds;
}


void PlayField::Update(float dt, const ScriptModule& scriptModule)
{
	// First move all game objects
	for (auto& player : players)
	{
		player.Move(dt, bounds);
	}
	for (auto& powerUp : powerUps)
	{
		powerUp.Move(dt, bounds);
	}
	for (auto& laser : lasers)
	{
		laser.Move(dt, bounds);
	}

	for (auto& alienShip : aliens)
	{
		Alien_Update(alienShip, dt, *this, config, scriptModule);
	}

	if (aliens.empty() == false)
	{
		for (auto& player : players)
		{
			player.ShootLasers(dt, *this, config.playerLaserVelocity, config.playerFireRate);
		}
	}

	// Collision detection
	collisionSpace->Clear();
	for (auto& player : players)
	{
		collisionSpace->Add(player.GetCollisionArea());
	}
	for (auto& alien : aliens)
	{
		collisionSpace->Add(Alien_GetCollider(alien));
	}
	for (auto& playerLaser : lasers)
	{
		collisionSpace->Add(playerLaser.GetCollider());
	}
	for (auto& powerUp : powerUps)
	{
		collisionSpace->Add(powerUp.GetCollisionArea());
	}
	for (auto& wall : walls)
	{
		collisionSpace->Add(wall.GetCollisionArea());
	}
	CheckCollisions();

	// Loop over game objects and delete dead ones. The "swap and pop_back" technique is used to delete items
	// in O(1) as their order is not important.
	Utils::RemoveElements(players, 
		[](PlayerShip& player){ return player.GetState() == PlayerShip::State::dead; } );
	Utils::RemoveElements(lasers, 
		[this](Laser& laser)
		{
			const bool res = laser.GetState() == Laser::State::dead;
			if (res && laser.GetOwnerId() == -1) ++availableAlienLasers; else ++availablePlayerLasers; 
			return res;
		} );
	Utils::RemoveElements(aliens, 
		[](Alien& alien){ return alien.state == Alien::State::dead; } );
	Utils::RemoveElements(powerUps, 
		[](PowerUp& powerUp){ return powerUp.GetState() == PowerUp::State::dead; } );
	Utils::RemoveElements(walls, 
		[](Wall& wall){ return wall.GetState() == Wall::State::dead; } );
	// Update and delete explosions in the same loop
	Utils::RemoveElements(explosions, [=](Explosion& explosion){ return explosion.Update(dt); } );
}


void PlayField::CheckCollisions()
{
	CollisionInfo collisions[64];
	const int nc = collisionSpace->Execute(collisions, _countof(collisions));
	CollisionInfo* c = collisions;

	using Callback = std::function<void(PlayField*, void*, void*)>;
	struct CallbackInfo
	{
		ColliderId id0;
		ColliderId id1;
		Callback   fnc;
	};
	static const CallbackInfo callbacks[] = 
	{
		{ ColliderId::player, ColliderId::alienLaser, &PlayField::Collision_PlayerVSLaser },
		{ ColliderId::player, ColliderId::powerUp, &PlayField::Collision_PlayerVSPowerUp },
		{ ColliderId::alien, ColliderId::alien, &PlayField::Collision_AlienVSAlien },
		{ ColliderId::alien, ColliderId::playerLaser, &PlayField::Collision_AlienVSLaser },
		{ ColliderId::alien, ColliderId::wall, &PlayField::Collision_AlienVSWall },
		{ ColliderId::playerLaser, ColliderId::alienLaser, &PlayField::Collision_LaserVSLaser },
		{ ColliderId::playerLaser, ColliderId::wall, &PlayField::Collision_LaserVSWall }
	};

	for (int i = 0; i < nc; ++i, ++c)
	{
		for (const auto& cbk : callbacks)
		{
			// Collision matrix
			if (c->id0 == cbk.id0 && c->id1 == cbk.id1)
			{
				cbk.fnc(this, c->ud0, c->ud1);
				break;
			}
			if (c->id0 == cbk.id1 && c->id1 == cbk.id0)
			{
				cbk.fnc(this, c->ud1, c->ud0);
				break;
			}
		}
	}
}


void PlayField::Collision_LaserVSLaser(void* ud0, void* ud1)
{

	Laser& playerLaser = *static_cast<Laser*>(ud0);
	Laser& alienLaser = *static_cast<Laser*>(ud1);
// Spawn explosion, kill this and the alien laser
	Vector2D pos = Lerp(playerLaser.GetPosition(), alienLaser.GetPosition(), 0.5f);
	AddExplosion(pos, config.explosionTimer);
	alienLaser.Destroy();
	playerLaser.Destroy();
}


void PlayField::Collision_AlienVSLaser(void* ud0, void* ud1)
{
	Alien& alien = *(Alien*)ud0;
	Laser& playerLaser = *(Laser*)ud1;
	playerLaser.Destroy();
	Alien_DecreaseHealth(alien); // kill the alien that we hit
	//Spawn explosion 
	AddScore( alien.state == Alien::State::normal ? 10 : 20, playerLaser.GetOwnerId());
	AddExplosion(alien.body.pos, config.explosionTimer);
	if (alien.state == Alien::State::dead)
	{
		if (rndFloat01(rGen) < config.powerUpRate)
		{
			SpawnRandomPowerUp(alien.body.pos);
		}
	}

	// FIXME
	//static int b = 0; if (!b++) {
	//SpawnPowerUp( { bounds.x/2, 10.f }, PowerUp::Type::bomb);
	//SpawnPowerUp( { bounds.x/2 - 10, 10.f }, PowerUp::Type::doubleFire);
//}
}


void PlayField::Collision_PlayerVSLaser(void* ud0, void* ud1)
{
	PlayerShip& player = *(PlayerShip*)ud0;
	Laser& alienLaser = *(Laser*)ud1;
	//Spawn explosion, destroy player and laser
	alienLaser.Destroy();
	if (! config.godMode)
	{
		player.Destroy();
	}
	AddExplosion(player.GetPosition(), config.explosionTimer);
}


void PlayField::Collision_PlayerVSAlien(void* ud0, void* ud1)
{
	PlayerShip& player = *(PlayerShip*)ud0;
	Alien& alien = *(Alien*)ud1;
	//Spawn explosion, destroy player and alien
	Alien_Destroy(alien);
	if (! config.godMode)
	{
		player.Destroy();
	}
	AddExplosion(alien.body.pos, config.explosionTimer);
}


void PlayField::Collision_PlayerVSPowerUp(void* ud0, void* ud1)
{
	PlayerShip& player = *(PlayerShip*)ud0;
	PowerUp& powerUp = *(PowerUp*)ud1;
	ActivatePowerUp(player, powerUp, messageLog);
	powerUp.Destroy();
}


void PlayField::Collision_LaserVSWall(void* ud0, void* ud1)
{
	Laser& playerLaser = *static_cast<Laser*>(ud0);
	Wall& wall = *static_cast<Wall*>(ud1);
	playerLaser.Destroy();
	wall.Hit();
	const Vector2D pos = { wall.GetPosition().x,  wall.GetPosition().y + 1.f };
	AddExplosion(pos, config.explosionTimer);
}


void PlayField::Collision_AlienVSWall(void* ud0, void* ud1)
{
	Alien& alien = *static_cast<Alien*>(ud0);
	Wall& wall = *static_cast<Wall*>(ud1);
	Alien_AvoidWall(alien, wall.GetPosition());
}


void PlayField::Collision_AlienVSAlien(void* ud0, void* ud1)
{
	Alien& alien0 = *static_cast<Alien*>(ud0);
	Alien& alien1 = *static_cast<Alien*>(ud1);
//	alien0.body.pos = alien0.body.prevPos;
}


void PlayField::ActivatePowerUp(PlayerShip& player, const PowerUp& powerUp, MessageLog& messageLog)
{
	switch (powerUp.GetType())
	{
	case PowerUp::speedBoost:
		messageLog.AddMessage("Speed Boost!");
		player.SetSpeedBoost(2);
		break;
	case PowerUp::fireBoost:
		messageLog.AddMessage("Fire Boost!");
		player.SetFireBoost(2);
		break;
	case PowerUp::doubleFire:
		messageLog.AddMessage("Double Fire!");
		player.SetDoubleFire();
		break;
	case PowerUp::tripleFire:
		messageLog.AddMessage("Triple Fire!");
		player.SetTripleFire();
		break;
	case PowerUp::invulnerability:
		messageLog.AddMessage("Invulnerability!");
		player.SetInvulnerable(config.powerUpInvulnerabilityTime);
		break;
	case PowerUp::bomb:
		messageLog.AddMessage("Bomb!");
		SpawnBomb();
		break;
	}
}


void PlayField::AddScore(int score, int playerId)
{
	for (auto& player : players)
	{ 
		if (player.GetId() == playerId)
		{
			player.AddScore(score);
			break;
		}
	}
}


void PlayField::DestroyAll()
{
	players.clear();
	aliens.clear();
	lasers.clear();
	explosions.clear();
	powerUps.clear();
	walls.clear();
}


void PlayField::DestroyWalls()
{
	walls.clear();
}


int PlayField::GetAvailablePlayerLasers() const 
{
	return availablePlayerLasers; 
}
