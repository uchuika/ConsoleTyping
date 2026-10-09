#include "AbstractScene.h"
#include <GL/glew.h>
#include "Game.h"
#include "Font.h"
#include "Texture.h"
#include "Shader.h"
#include "Keyboard.h"
#include "Palette.h"

namespace
{
	const char* FontRegular = "Assets/Fonts/PlemolJP-Regular.ttf";
	const char* FontBold = "Assets/Fonts/PlemolJP-Bold.ttf";
}

AbstractScene::AbstractScene(IOnSceneChangedListener* impl, const Parameter& parameter, Game* game)
:mListener(impl)
,mGame(game)
,mFont(game->GetFont(FontRegular))
,mBoldFont(game->GetFont(FontBold))
{
	mFrameTitle = AddText(u8"TYPE://TERMINAL", 18, true);
	mFrameRight = nullptr;

	SetFrameInfo(u8"文化祭展示  ·  local", {
		{ u8"↑↓", u8"選択" },
		{ u8"Enter", u8"決定" },
		{ u8"Esc", u8"戻る" },
		{ u8"Alt+Enter", u8"全画面 / ウィンドウ" },
	});
}

void AbstractScene::SetFrameInfo(const std::string& rightLabel,
	const std::vector<std::pair<std::string, std::string>>& footer)
{
	mFrameRight = AddText(rightLabel, 15);
	mFooter.clear();
	for (const auto& item : footer)
	{
		mFooter.emplace_back(AddText(item.first, 16), AddText(item.second, 16));
	}
}

AbstractScene::~AbstractScene()
{
}

Text* AbstractScene::AddText(const std::string& utf8Text, int pointSize, bool bold)
{
	mTexts.emplace_back(new Text(bold ? mBoldFont : mFont, utf8Text, pointSize));
	return mTexts.back().get();
}

bool AbstractScene::IsPressed(int scancode) const
{
	return Keyboard::getIns()->GetPressingCount(scancode) == 1;
}

bool AbstractScene::IsConfirmPressed() const
{
	Keyboard* keyboard = Keyboard::getIns();
	return keyboard->GetPressingCount(SDL_SCANCODE_RETURN) == 1
		&& keyboard->GetPressingCount(SDL_SCANCODE_LALT) == 0
		&& keyboard->GetPressingCount(SDL_SCANCODE_RALT) == 0;
}

void AbstractScene::DrawTerminalFrame(Shader* shader) const
{
	const float w = mGame->GetScreenWidth();
	const float h = mGame->GetScreenHeight();

	// 外側とパネル
	DrawRect(shader, 0.0f, 0.0f, w, h, Palette::Outside);
	DrawRect(shader, 32.0f, 24.0f, 1216.0f, 672.0f, Palette::Panel);
	DrawBorder(shader, 32.0f, 24.0f, 1216.0f, 672.0f, Palette::Line);

	// タイトルバー
	DrawCircle(shader, 56.0f, 48.0f, 12.0f, Palette::DotRed);
	DrawCircle(shader, 74.0f, 48.0f, 12.0f, Palette::DotYellow);
	DrawCircle(shader, 92.0f, 48.0f, 12.0f, Palette::DotGreen);
	DrawText(shader, mFrameTitle, 116.0f, 36.0f, Palette::Cyan);
	DrawText(shader, mFrameRight, 1216.0f - mFrameRight->GetWidth(), 38.0f, Palette::TextFaint);
	DrawRect(shader, 33.0f, 72.0f, 1214.0f, 1.0f, Palette::Line);

	// フッター
	DrawRect(shader, 64.0f, 640.0f, 1152.0f, 1.0f, Palette::Line);
	float x = 64.0f;
	for (auto& item : mFooter)
	{
		DrawText(shader, item.first, x, 656.0f, Palette::Text);
		x += item.first->GetWidth() + 8.0f;
		DrawText(shader, item.second, x, 656.0f, Palette::TextMute);
		x += item.second->GetWidth() + 28.0f;
	}
}

void AbstractScene::DrawText(Shader* shader, const Text* text, float x, float y,
	const Vector3& color, float alpha) const
{
	if (!text || !text->GetTexture())
	{
		return;
	}
	DrawTexture(shader, text->GetTexture(), x, y,
		static_cast<float>(text->GetWidth()), static_cast<float>(text->GetHeight()),
		color, alpha);
}

void AbstractScene::DrawRect(Shader* shader, float x, float y, float w, float h,
	const Vector3& color, float alpha) const
{
	DrawTexture(shader, mGame->GetWhiteTexture(), x, y, w, h, color, alpha);
}

void AbstractScene::DrawBorder(Shader* shader, float x, float y, float w, float h,
	const Vector3& color) const
{
	DrawRect(shader, x, y, w, 1.0f, color);
	DrawRect(shader, x, y + h - 1.0f, w, 1.0f, color);
	DrawRect(shader, x, y + 1.0f, 1.0f, h - 2.0f, color);
	DrawRect(shader, x + w - 1.0f, y + 1.0f, 1.0f, h - 2.0f, color);
}

void AbstractScene::DrawCircle(Shader* shader, float x, float y, float diameter,
	const Vector3& color) const
{
	DrawTexture(shader, mGame->GetCircleTexture(), x, y, diameter, diameter, color, 1.0f);
}

void AbstractScene::DrawTexture(Shader* shader, Texture* texture,
	float x, float y, float w, float h,
	const Vector3& color, float alpha) const
{
	// 論理座標 (左上原点) から、画面中央が原点の座標へ変換
	const float cx = x + w * 0.5f - mGame->GetScreenWidth() * 0.5f;
	const float cy = mGame->GetScreenHeight() * 0.5f - (y + h * 0.5f);

	Matrix4 world = Matrix4::CreateScale(w, h, 1.0f)
		* Matrix4::CreateTranslation(Vector3(cx, cy, 0.0f));
	shader->SetMatrixUniform("uWorldTransform", world);
	shader->SetColorUniform("uColor", color, alpha);
	texture->SetActive();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
