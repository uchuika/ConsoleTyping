#pragma once

#include "Singleton.h"
#include <array>
#include <SDL.h>

// キーの押下/解放フレーム数を数える。キーコードは SDL_SCANCODE_* を使う
class Keyboard : public Singleton<Keyboard>
{
	Keyboard();
	friend Singleton<Keyboard>;

public:
	// SDL_PollEvent の後にフレームごとに1回呼ぶ
	void Update();

	// scancode のキーが押されているフレーム数を取得 (押した瞬間は 1)
	int GetPressingCount(int scancode) const;
	// scancode のキーが離されているフレーム数を取得
	int GetReleasingCount(int scancode) const;

private:
	bool isAvailableCode(int scancode) const;

	std::array<int, SDL_NUM_SCANCODES> mPressingCount;
	std::array<int, SDL_NUM_SCANCODES> mReleasingCount;
};
