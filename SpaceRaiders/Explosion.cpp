#include "stdafx.h"
#include "Explosion.h"
#include "Images.h"
#include "Collision.h"
#include <algorithm>
#include <cassert>


Explosion::Explosion(const Vector2D& initialPos, float timer) :
	pos { initialPos },
	visual { ImageId::explosion, Color::yellowIntense, },
	timer { timer }
{}


const Vector2D& Explosion::GetPosition() const
{
	return pos;
}


const Visual& Explosion::GetVisual() const
{
	return visual;
}


bool Explosion::Update(float dt)
{
	assert(timer > 0.f);
	timer -= dt;
	// Delete if timer <= 0.f
	return (timer <= 0.f);
}
