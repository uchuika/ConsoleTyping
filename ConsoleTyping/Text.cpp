#include "Text.h"
#include <cmath>
#include "Font.h"
#include "Texture.h"

Text::Text(Font* font, const std::string& text, int pointSize)
:mFont(font)
,mPointSize(pointSize)
,mTexture(nullptr)
,mScaleVersion(0)
{
	SetText(text);
}

Text::~Text()
{
	Release();
}

void Text::SetText(const std::string& text)
{
	Release();
	mText = text;
	mScaleVersion = mFont->GetScaleVersion();
	mTexture = mFont->RenderText(mText, mPointSize);
}

Texture* Text::GetTexture() const
{
	// 拡大率が変わっていたら、今の拡大率で作り直す
	if (mScaleVersion != mFont->GetScaleVersion())
	{
		Release();
		mScaleVersion = mFont->GetScaleVersion();
		mTexture = mFont->RenderText(mText, mPointSize);
	}
	return mTexture;
}

int Text::GetWidth() const
{
	return static_cast<int>(std::lround(GetDrawWidth()));
}

int Text::GetHeight() const
{
	return static_cast<int>(std::lround(GetDrawHeight()));
}

float Text::GetDrawWidth() const
{
	Texture* texture = GetTexture();
	return texture ? texture->GetWidth() / mFont->GetScale() : 0.0f;
}

float Text::GetDrawHeight() const
{
	Texture* texture = GetTexture();
	return texture ? texture->GetHeight() / mFont->GetScale() : 0.0f;
}

void Text::Release() const
{
	if (mTexture)
	{
		mTexture->Unload();
		delete mTexture;
		mTexture = nullptr;
	}
}
