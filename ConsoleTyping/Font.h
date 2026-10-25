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

	// 画面の拡大率。pointSize は論理サイズで、実際には pointSize * 拡大率 で描画する
	void SetScale(float scale);
	float GetScale() const { return mScale; }
	// 拡大率が変わるたびに増える。Text が作り直しの要否を判断する
	int GetScaleVersion() const { return mScaleVersion; }

	// UTF-8 文字列を白で描画したテクスチャを作る (呼び出し側が Unload/delete する)
	class Texture* RenderText(const std::string& utf8Text, int pointSize);

	// 描画したときの幅 (論理ピクセル)。開けなければ 0
	int GetTextWidth(const std::string& utf8Text, int pointSize);

private:
	// pixelSize は拡大率を掛けた実際のサイズ
	TTF_Font* GetFontData(int pixelSize);
	int ToPixelSize(int pointSize) const;

	std::string mFileName;
	float mScale = 1.0f;
	int mScaleVersion = 0;
	std::unordered_map<int, TTF_Font*> mFontData;
};
