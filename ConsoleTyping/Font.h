#pragma once

#include <string>
#include <unordered_map>
#include <SDL_ttf.h>

// TTF フォント。ポイントサイズごとに遅延して開く
class Font
{
public:
	Font();
	~Font();

	bool Load(const std::string& fileName);
	void Unload();

	// UTF-8 文字列を白で描画したテクスチャを作る (呼び出し側が Unload/delete する)
	class Texture* RenderText(const std::string& utf8Text, int pointSize);

private:
	TTF_Font* GetFontData(int pointSize);

	std::string mFileName;
	std::unordered_map<int, TTF_Font*> mFontData;
};
