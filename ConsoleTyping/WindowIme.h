#pragma once

#include <SDL.h>

// ウィンドウの IME を無効にする。
// 日本語入力が ON でも、キー入力を IME に奪われず物理キー (scancode) のまま受け取るため。
// Windows 以外では何もしない。
void DisableIme(SDL_Window* window);
