#include "MainMenuScene.h"
#include "Palette.h"

namespace
{
	const float ItemX = 72.0f;
	const float ItemY = 232.0f;
	const float ItemW = 1136.0f;
	const float ItemH = 84.0f;
	const float ItemStride = 94.0f;
}

MainMenuScene::MainMenuScene(IOnSceneChangedListener* impl, const Parameter& parameter, Game* game)
:AbstractScene(impl, parameter, game)
,mSelected(1)
{
	mPrompt = AddText(u8"PS C:\\festival>", 18);
	mSubtitle = AddText(u8"ターミナル風タイピング     –   1分間、正しく打てたローマ字を数える", 20);
	mSelectLabel = AddText(u8"select", 14);
	mSelectHint = AddText(u8"上下キーで選び、Enter で決定", 14);
	mExitLabel = AddText(u8"終了", 13);
	mExitLine1 = AddText(u8"Alt+Enter でウィンドウ表示にしてから、閉じるボタンで終了", 15);
	mExitLine2 = AddText(u8"メニューに「ゲーム終了」はない", 13);

	mItems.push_back({ EScene::Ranking,
		AddText(u8"ランキング", 30),
		AddText(u8"保存済みの記録を、打てた文字数の多い順に見る", 16) });
	mItems.push_back({ EScene::Typing,
		AddText(u8"タイピング開始", 30),
		AddText(u8"名前を入れて 3 カウント、60 秒スタート", 16) });
}

void MainMenuScene::update(float deltaTime)
{
	if (IsPressed(SDL_SCANCODE_UP) && mSelected > 0)
	{
		mSelected--;
	}
	if (IsPressed(SDL_SCANCODE_DOWN) && mSelected < static_cast<int>(mItems.size()) - 1)
	{
		mSelected++;
	}
	if (IsConfirmPressed())
	{
		mListener->onSceneChanged(mItems[mSelected].scene, Parameter(), false);
	}
}

void MainMenuScene::draw(Shader* shader) const
{
	DrawTerminalFrame(shader);

	DrawText(shader, mPrompt, 80.0f, 100.0f, Palette::Green);
	DrawText(shader, mSubtitle, 80.0f, 132.0f, Palette::Text);
	DrawText(shader, mSelectLabel, 80.0f, 190.0f, Palette::Purple);
	DrawText(shader, mSelectHint, 140.0f, 190.0f, Palette::TextFaint);

	for (size_t i = 0; i < mItems.size(); i++)
	{
		DrawItem(shader, mItems[i], ItemY + ItemStride * i, static_cast<int>(i) == mSelected);
	}

	DrawText(shader, mExitLabel, 80.0f, 454.0f, Palette::TextFaint);
	DrawText(shader, mExitLine1, 140.0f, 450.0f, Palette::TextFaint);
	DrawText(shader, mExitLine2, 140.0f, 476.0f, Palette::TextDark);
}

void MainMenuScene::DrawItem(Shader* shader, const Item& item, float y, bool selected) const
{
	if (selected)
	{
		DrawRect(shader, ItemX, y, ItemW, ItemH, Palette::SelectBg);
		DrawBorder(shader, ItemX, y, ItemW, ItemH, Palette::SelectLine);
		DrawRect(shader, ItemX, y + 16.0f, 4.0f, ItemH - 32.0f, Palette::Purple);
	}
	DrawText(shader, item.title, ItemX + 36.0f, y + 6.0f,
		selected ? Palette::TextBright : Palette::TextDim);
	DrawText(shader, item.description, ItemX + 36.0f, y + 52.0f, Palette::TextMute);
}

EScene MainMenuScene::GetScene() const
{
	return EScene::MainMenu;
}
