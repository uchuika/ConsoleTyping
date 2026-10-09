// ----------------------------------------------------------------
// Based on Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Texture.h"
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_image.h>

Texture::Texture()
:mTextureID(0)
,mWidth(0)
,mHeight(0)
{

}

Texture::~Texture()
{

}

bool Texture::Load(const std::string& fileName)
{
	SDL_Surface* surf = IMG_Load(fileName.c_str());
	if (!surf)
	{
		SDL_Log("Failed to load image %s: %s", fileName.c_str(), IMG_GetError());
		return false;
	}

	bool result = CreateFromSurface(surf);
	SDL_FreeSurface(surf);
	return result;
}

bool Texture::CreateFromSurface(SDL_Surface* surface)
{
	// GL_RGBA / GL_UNSIGNED_BYTE と並びが一致する形式へ変換
	SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ABGR8888, 0);
	if (!converted)
	{
		SDL_Log("Failed to convert surface: %s", SDL_GetError());
		return false;
	}

	mWidth = converted->w;
	mHeight = converted->h;

	// 乗算済みアルファへ変換 (ブレンドは GL_ONE, GL_ONE_MINUS_SRC_ALPHA)
	Uint8* row = static_cast<Uint8*>(converted->pixels);
	for (int y = 0; y < converted->h; y++)
	{
		Uint8* p = row;
		for (int x = 0; x < converted->w; x++)
		{
			const Uint8 a = p[3];
			p[0] = static_cast<Uint8>(p[0] * a / 255);
			p[1] = static_cast<Uint8>(p[1] * a / 255);
			p[2] = static_cast<Uint8>(p[2] * a / 255);
			p += 4;
		}
		row += converted->pitch;
	}

	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, converted->pitch / 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0,
				 GL_RGBA, GL_UNSIGNED_BYTE, converted->pixels);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

	SDL_FreeSurface(converted);

	// Enable bilinear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return true;
}

void Texture::Unload()
{
	glDeleteTextures(1, &mTextureID);
}

void Texture::SetActive()
{
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}
