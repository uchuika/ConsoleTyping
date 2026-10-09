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
	// Uploads the surface as a premultiplied-alpha RGBA texture (surface is not freed)
	bool CreateFromSurface(struct SDL_Surface* surface);
	void Unload();
	
	void SetActive();
	
	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }
private:
	// OpenGL ID of this texture
	unsigned int mTextureID;
	// Width/height of the texture
	int mWidth;
	int mHeight;
};
