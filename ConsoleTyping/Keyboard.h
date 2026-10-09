#pragma once

#include "Singleton.h"
#include <array>
#include <vector>
#include <SDL.h>

// キーの押下/解放フレーム数を数える。キーコードは SDL_SCANCODE_* を使う
class Keyboard : public Singleton<Keyboard>
{
	Keyboard();
	friend Singleton<Keyboard>;

public:
	// フレームの最初 (SDL_PollEvent の前) に 1 回呼ぶ。押下イベントのバッファを空にする
	void BeginFrame();
	// キーが押された瞬間 (キーリピートを除く) にイベントから呼ぶ
	void OnKeyDown(int scancode);
	// このフレームに押されたキーを押した順に返す。1 フレームに複数押されても取りこぼさない
	const std::vector<int>& GetKeyDowns() const { return mKeyDowns; }

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
	std::vector<int> mKeyDowns;
};
