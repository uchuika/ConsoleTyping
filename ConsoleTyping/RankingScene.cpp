#include "RankingScene.h"
#include "Palette.h"

RankingScene::RankingScene(IOnSceneChangedListener* impl, const Parameter& parameter, Game* game)
:AbstractScene(impl, parameter, game)
{
	mTitle = AddText(u8"ランキング (準備中)", 30);
	mHint = AddText(u8"Esc でメインメニューへ戻る", 16);
}

void RankingScene::update(float deltaTime)
{
	if (IsPressed(SDL_SCANCODE_ESCAPE))
	{
		mListener->onSceneChanged(true);
	}
}

void RankingScene::draw(Shader* shader) const
{
	DrawTerminalFrame(shader);
	DrawText(shader, mTitle, 80.0f, 120.0f, Palette::TextBright);
	DrawText(shader, mHint, 80.0f, 176.0f, Palette::TextMute);
}

EScene RankingScene::GetScene() const
{
	return EScene::Ranking;
}
