#include "Keyboard.h"

Keyboard::Keyboard()
{
	mPressingCount.fill(0);
	mReleasingCount.fill(0);
}

char Keyboard::ToChar(int scancode)
{
	if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z)
	{
		return static_cast<char>('a' + (scancode - SDL_SCANCODE_A));
	}
	if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_9)
	{
		return static_cast<char>('1' + (scancode - SDL_SCANCODE_1));
	}
	if (scancode == SDL_SCANCODE_0)
	{
		return '0';
	}
	if (scancode == SDL_SCANCODE_MINUS)
	{
		return '-';
	}
	return 0;
}

void Keyboard::BeginFrame()
{
	mKeyDowns.clear();
}

void Keyboard::OnKeyDown(int scancode)
{
	if (isAvailableCode(scancode))
	{
		mKeyDowns.push_back(scancode);
	}
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
