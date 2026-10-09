#pragma once

#include <string>

// 文字列から作ったテクスチャを持つ。色は描画時に指定する
class Text
{
public:
	Text(class Font* font, const std::string& text, int pointSize);
	~Text();
	Text(const Text&) = delete;
	Text& operator=(const Text&) = delete;

	void SetText(const std::string& text);

	class Texture* GetTexture() const { return mTexture; }
	int GetWidth() const;
	int GetHeight() const;

private:
	void Release();

	class Font* mFont;
	std::string mText;
	int mPointSize;
	class Texture* mTexture;
};
