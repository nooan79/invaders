#pragma once

#include "Vector2D.h"
#include "RenderItem.h"


class PlayField;
struct CollisionArea;


class PowerUp
{
public:

	enum class State
	{
		normal,
		dead
	};

	enum Type
	{
		speedBoost,
		fireBoost,
		tripleFire,
		invulnerability,
		count
	};

	PowerUp(const Vector2D& initialPos, const Visual& visual, const Vector2D& size, float velocity, Type type);

	const Vector2D& GetPosition() const;
	CollisionArea GetCollisionArea() const;
	State GetState() const;
	Type GetType() const;
	void Move(float dt, const Vector2D& worldBounds);
	void Destroy();
	const Visual& GetVisual() const;

private:

	Vector2D   pos;
	Vector2D   prevPos;
	float      velocity;
	Type       type;
	Visual     visual;
	Vector2D   size;
	State      state;
};
