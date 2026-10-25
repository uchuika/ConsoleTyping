#include "Font.h"
#include <algorithm>
#include <cmath>
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
	return GetFontData(ToPixelSize(16)) != nullptr;
}

void Font::Unload()
{
	for (auto& font : mFontData)
	{
		TTF_CloseFont(font.second);
	}
	mFontData.clear();
}

void Font::SetScale(float scale)
{
	if (scale <= 0.0f || scale == mScale)
	{
		return;
	}
	mScale = scale;
	mScaleVersion++;
	// 古い拡大率のサイズを持ち続けないよう開き直す
	Unload();
}

int Font::ToPixelSize(int pointSize) const
{
	return std::max(1, static_cast<int>(std::lround(pointSize * mScale)));
}

TTF_Font* Font::GetFontData(int pixelSize)
{
	auto iter = mFontData.find(pixelSize);
	if (iter != mFontData.end())
	{
		return iter->second;
	}

	TTF_Font* font = TTF_OpenFont(mFileName.c_str(), pixelSize);
	if (!font)
	{
		SDL_Log("Failed to load font %s in size %d: %s",
			mFileName.c_str(), pixelSize, TTF_GetError());
		return nullptr;
	}
	mFontData.emplace(pixelSize, font);
	return font;
}

int Font::GetTextWidth(const std::string& utf8Text, int pointSize)
{
	TTF_Font* font = GetFontData(ToPixelSize(pointSize));
	int width = 0;
	if (!font || TTF_SizeUTF8(font, utf8Text.c_str(), &width, nullptr) != 0)
	{
		return 0;
	}
	return static_cast<int>(std::lround(width / mScale));
}

Texture* Font::RenderText(const std::string& utf8Text, int pointSize)
{
	// 空文字列は NULL サーフェスになるので描画しない
	if (utf8Text.empty())
	{
		return nullptr;
	}

	TTF_Font* font = GetFontData(ToPixelSize(pointSize));
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
