#pragma once

#include <string>

// 文字列から作ったテクスチャを持つ。色は描画時に指定する
// テクスチャは画面の拡大率に合わせて作られ、拡大率が変わると GetTexture で作り直す
class Text
{
public:
	Text(class Font* font, const std::string& text, int pointSize);
	~Text();
	Text(const Text&) = delete;
	Text& operator=(const Text&) = delete;

	void SetText(const std::string& text);

	class Texture* GetTexture() const;
	// 論理座標での大きさ (レイアウト計算用)
	int GetWidth() const;
	int GetHeight() const;
	// 丸めない論理サイズ (描画用)。テクスチャのピクセル数 / 拡大率
	float GetDrawWidth() const;
	float GetDrawHeight() const;

private:
	void Release() const;

	class Font* mFont;
	std::string mText;
	int mPointSize;
	mutable class Texture* mTexture;
	mutable int mScaleVersion;
};
