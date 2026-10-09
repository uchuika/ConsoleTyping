#include "Keyboard.h"

Keyboard::Keyboard()
{
	mPressingCount.fill(0);
	mReleasingCount.fill(0);
}

void Keyboard::Update()
{
	const Uint8* keyState = SDL_GetKeyboardState(nullptr);
	for (int i = 0; i < SDL_NUM_SCANCODES; i++)
	{
		if (keyState[i] != 0)
		{
			mReleasingCount[i] = 0;
			mPressingCount[i]++;
		}
		else
		{
			mPressingCount[i] = 0;
			mReleasingCount[i]++;
		}
	}
}

int Keyboard::GetPressingCount(int scancode) const
{
	if (!isAvailableCode(scancode))
	{
		return -1;
	}
	return mPressingCount[scancode];
}

int Keyboard::GetReleasingCount(int scancode) const
{
	if (!isAvailableCode(scancode))
	{
		return -1;
	}
	return mReleasingCount[scancode];
}

bool Keyboard::isAvailableCode(int scancode) const
{
	return 0 <= scancode && scancode < SDL_NUM_SCANCODES;
}
