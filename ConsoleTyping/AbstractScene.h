#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "SDL.h"
#include "EScene.h"
#include "Parameter.h"
#include "IOnSceneChangedListener.h"
#include "Math.h"
#include "Text.h"

// 全シーンの基底クラス。Game はシーンのスタックを持ち、一番上のシーンだけを動かす。
// 描画座標は論理解像度 (左上原点、y 下向き、ピクセル) で指定する。
class AbstractScene
{
public:
	AbstractScene(IOnSceneChangedListener* impl, const Parameter& parameter, class Game* game);
	virtual ~AbstractScene();

	virtual void update(float deltaTime) = 0;
	virtual void draw(class Shader* shader) const = 0;
	virtual EScene GetScene() const = 0;

protected:
	// シーンが所有するテキストを作る (シーン破棄時に解放される)
	Text* AddText(const std::string& utf8Text, int pointSize, bool bold = false);

	// 押した瞬間か
	bool IsPressed(int scancode) const;
	// Enter で決定した瞬間か (Alt+Enter は含まない)
	bool IsConfirmPressed() const;

	// 全シーン共通のウィンドウ風の外枠 (タイトルバーと操作説明のフッター)
	void DrawTerminalFrame(class Shader* shader) const;

	void DrawText(class Shader* shader, const Text* text, float x, float y,
		const Vector3& color, float alpha = 1.0f) const;
	void DrawRect(class Shader* shader, float x, float y, float w, float h,
		const Vector3& color, float alpha = 1.0f) const;
	// 1px の枠線
	void DrawBorder(class Shader* shader, float x, float y, float w, float h,
		const Vector3& color) const;
	void DrawCircle(class Shader* shader, float x, float y, float diameter,
		const Vector3& color) const;

	IOnSceneChangedListener* mListener;
	class Game* mGame;
	class Font* mFont;
	class Font* mBoldFont;

private:
	void DrawTexture(class Shader* shader, class Texture* texture,
		float x, float y, float w, float h,
		const Vector3& color, float alpha) const;

	std::vector<std::unique_ptr<Text>> mTexts;

	// 外枠のテキスト
	Text* mFrameTitle;
	Text* mFrameRight;
	// フッター (キー, 説明) の組
	std::vector<std::pair<Text*, Text*>> mFooter;
};
