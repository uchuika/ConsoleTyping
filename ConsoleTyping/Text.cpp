#include "Text.h"
#include "Font.h"
#include "Texture.h"

Text::Text(Font* font, const std::string& text, int pointSize)
:mFont(font)
,mPointSize(pointSize)
,mTexture(nullptr)
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
	mTexture = mFont->RenderText(mText, mPointSize);
}

int Text::GetWidth() const
{
	return mTexture ? mTexture->GetWidth() : 0;
}

int Text::GetHeight() const
{
	return mTexture ? mTexture->GetHeight() : 0;
}

void Text::Release()
{
	if (mTexture)
	{
		mTexture->Unload();
		delete mTexture;
		mTexture = nullptr;
	}
}
