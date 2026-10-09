#include "WindowIme.h"
#include <SDL_syswm.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <imm.h>
#endif

void DisableIme(SDL_Window* window)
{
	// SDL のテキスト入力 (IME 候補ウィンドウなど) を止める
	SDL_StopTextInput();

#ifdef _WIN32
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if (SDL_GetWindowWMInfo(window, &info) && info.subsystem == SDL_SYSWM_WINDOWS)
	{
		// このウィンドウから入力コンテキストを外すと、IME は変換を始めない
		ImmAssociateContext(info.info.win.window, nullptr);
	}
#endif
}
