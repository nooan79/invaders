#include "Input.h"
#include <windows.h>


namespace
{


int prevKeyState[numKeyCodes] = { -1 };
int keyState[numKeyCodes] = { -1 };
const int keyCodeToVKey[numKeyCodes] =
{
	VK_ESCAPE,
	VK_RETURN,
	VK_SPACE,
	VK_LEFT,
	VK_RIGHT,
	VK_UP,
	VK_DOWN,
	'1',
	'2',
	'3',
	'4',
	'5',
	'A',
	'D'
};

}


void UpdateKeyStates()
{
	std::memcpy(prevKeyState, keyState, sizeof(prevKeyState));
	for (size_t i = 0; i < numKeyCodes; ++i)
	{
		keyState[i] = (GetAsyncKeyState(keyCodeToVKey[i]) & 0x8000) ? 1 : 0;
	}
}


bool KeyPressed(KeyCode keyCode)
{
	return (keyState[(int)keyCode] == 1);
}

bool KeyJustPressed(KeyCode keyCode)
{
	return (keyState[(int)keyCode] == 1) && (prevKeyState[(int)keyCode] == 0);
}

bool KeyJustReleased(KeyCode keyCode)
{
	return (keyState[(int)keyCode] == 0) && (prevKeyState[(int)keyCode] == 1);
}


bool AnyKeyJustPressed()
{
	bool res = false;
	for (size_t i = 0; i < numKeyCodes; ++i)
	{
		if (KeyJustPressed((KeyCode)i))
		{
			res = true;
			break;
		}
	}
	return res;
}


RndInput::RndInput(std::default_random_engine& rGen) : 
	rGen {rGen}, 
	accumTime {0},
	state {0},
	rndInt { { 30, 30, 30 }}
{}


void RndInput::Update(float dt)
{
	accumTime += dt;
	if (accumTime > 0.25f)
	{
		accumTime = 0.f;
		state =	rndInt(rGen);
	}
}


bool RndInput::Left() const
{ 
	return state == 0;
}

bool RndInput::Right() const
{ 
	return state == 2; 
}


KeyboardInput::KeyboardInput(KeyCode left, KeyCode right) : 
	left { left },
	right { right }
{}


void KeyboardInput::Update(float /*dt*/)
{}


bool KeyboardInput::Left() const
{ 
	return KeyPressed(left);
}


bool KeyboardInput::Right() const
{ 
	return KeyPressed(right);
}
