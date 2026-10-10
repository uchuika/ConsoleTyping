// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <string>

class Texture
{
public:
	Texture();
	~Texture();
	
	bool Load(const std::string& fileName);
	// サーフェスを事前乗算アルファのRGBAテクスチャとしてアップロードする(サーフェスは解放しない)
	bool CreateFromSurface(struct SDL_Surface* surface);
	void Unload();
	
	void SetActive();
	
	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }
private:
	// このテクスチャのOpenGL ID
	unsigned int mTextureID;
	// テクスチャの幅/高さ
	int mWidth;
	int mHeight;
};
