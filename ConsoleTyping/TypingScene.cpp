#include "TypingScene.h"
#include "Palette.h"

TypingScene::TypingScene(IOnSceneChangedListener* impl, const Parameter& parameter, Game* game)
:AbstractScene(impl, parameter, game)
{
	mTitle = AddText(u8"タイピング (準備中)", 30);
	mHint = AddText(u8"Esc でメインメニューへ戻る", 16);
}

void TypingScene::update(float deltaTime)
{
	if (IsPressed(SDL_SCANCODE_ESCAPE))
	{
		mListener->onSceneChanged(true);
	}
}

void TypingScene::draw(Shader* shader) const
{
	DrawTerminalFrame(shader);
	DrawText(shader, mTitle, 80.0f, 120.0f, Palette::TextBright);
	DrawText(shader, mHint, 80.0f, 176.0f, Palette::TextMute);
}

EScene TypingScene::GetScene() const
{
	return EScene::Typing;
}
