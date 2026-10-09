#include "Font.h"
#include <SDL.h>
#include "Texture.h"

Font::Font()
{
}

Font::~Font()
{
}

bool Font::Load(const std::string& fileName)
{
	mFileName = fileName;
	// 開けるかどうかをここで確認する
	return GetFontData(16) != nullptr;
}

void Font::Unload()
{
	for (auto& font : mFontData)
	{
		TTF_CloseFont(font.second);
	}
	mFontData.clear();
}

TTF_Font* Font::GetFontData(int pointSize)
{
	auto iter = mFontData.find(pointSize);
	if (iter != mFontData.end())
	{
		return iter->second;
	}

	TTF_Font* font = TTF_OpenFont(mFileName.c_str(), pointSize);
	if (!font)
	{
		SDL_Log("Failed to load font %s in size %d: %s",
			mFileName.c_str(), pointSize, TTF_GetError());
		return nullptr;
	}
	mFontData.emplace(pointSize, font);
	return font;
}

Texture* Font::RenderText(const std::string& utf8Text, int pointSize)
{
	// 空文字列は NULL サーフェスになるので描画しない
	if (utf8Text.empty())
	{
		return nullptr;
	}

	TTF_Font* font = GetFontData(pointSize);
	if (!font)
	{
		return nullptr;
	}

	const SDL_Color white = { 255, 255, 255, 255 };
	SDL_Surface* surf = TTF_RenderUTF8_Blended(font, utf8Text.c_str(), white);
	if (!surf)
	{
		SDL_Log("Failed to render text: %s", TTF_GetError());
		return nullptr;
	}

	Texture* texture = new Texture();
	if (!texture->CreateFromSurface(surf))
	{
		delete texture;
		texture = nullptr;
	}
	SDL_FreeSurface(surf);
	return texture;
}
