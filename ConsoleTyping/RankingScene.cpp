#include "RankingScene.h"
#include <algorithm>
#include <ctime>
#include "Palette.h"

namespace
{
	const size_t MaxRanks = 20;
	const size_t VisibleRows = 10;

	const float RowTop = 202.0f;
	const float RowStride = 36.0f;
	const float RowHeight = 32.0f;
	const float RowX = 72.0f;
	const float RowWidth = 1136.0f;

	const float ColRank = 88.0f;
	const float ColName = 192.0f;
	const float ColChars = 576.0f;
	const float ColWhen = 784.0f;
	const float ColArrow = 900.0f;

	const int RowFontSize = 22;
}

RankingScene::RankingScene(IOnSceneChangedListener* impl, const Parameter& parameter, Game* game)
:AbstractScene(impl, parameter, game)
,mScroll(0)
{
	SetFrameInfo(u8"ranking  ·  local", {});

	mPrompt = AddText("PS C:\\festival>", 18);
	mCommand = AddText("rank", 18);
	mSubtitle = AddText(u8"", 20);
	mHeaderRank = AddText(u8"順位", 16);
	mHeaderName = AddText(u8"ニックネーム", 16);
	mHeaderChars = AddText(u8"文字数", 16);
	mHeaderWhen = AddText(u8"記録", 16);
	mThisPlay = AddText(u8"←  このプレイ", 16);
	mEmpty = AddText(u8"まだ記録がありません。タイピングで最初の記録を作ろう", 20);
	mRange = AddText("", 14);
	mNote = AddText(u8"", 14);
	mFile = AddText("", 14);

	// 今回のプレイを強調するのは、タイピング画面から直接来たときだけ
	const int highlightId = (parameter.get("highlight") == 1) ? ScoreBoard::getIns()->GetLastAddedId() : -1;

	size_t highlightIndex = 0;
	std::vector<ScoreRecord> records = ScoreBoard::getIns()->GetSorted(MaxRanks);
	for (size_t i = 0; i < records.size(); i++)
	{
		const ScoreRecord& record = records[i];
		const bool highlighted = (record.id == highlightId);
		if (highlighted)
		{
			highlightIndex = i;
		}
		mRows.push_back({
			record,
			highlighted,
			AddText(std::to_string(i + 1), RowFontSize),
			AddText(record.name, RowFontSize),
			AddText(std::to_string(record.chars), RowFontSize),
			AddText(FormatWhen(record, highlighted), RowFontSize),
		});
	}

	// 強調する行が最初の画面の外なら、見える位置までスクロールしておく
	if (highlightIndex >= VisibleRows)
	{
		mScroll = highlightIndex + 1 - VisibleRows;
	}
	UpdateRangeText();
}

std::string RankingScene::FormatWhen(const ScoreRecord& record, bool highlighted) const
{
	if (highlighted)
	{
		return u8"いま";
	}

	std::time_t played = static_cast<std::time_t>(record.playedAt);
	std::time_t now = std::time(nullptr);
	std::tm playedTm = {};
	std::tm nowTm = {};
	localtime_s(&playedTm, &played);
	localtime_s(&nowTm, &now);
	if (playedTm.tm_year == nowTm.tm_year && playedTm.tm_yday == nowTm.tm_yday)
	{
		return u8"今日";
	}
	return std::to_string(playedTm.tm_mon + 1) + "/" + std::to_string(playedTm.tm_mday);
}

void RankingScene::UpdateRangeText()
{
	if (mRows.empty())
	{
		mRange->SetText("");
		return;
	}
	const size_t last = std::min(mScroll + VisibleRows, mRows.size());
	mRange->SetText(std::string(u8"表示  ") + std::to_string(mScroll + 1) + "-" + std::to_string(last)
		+ " / " + std::to_string(mRows.size()));
}

void RankingScene::update(float deltaTime)
{
	if (IsPressed(SDL_SCANCODE_ESCAPE))
	{
		mListener->onSceneChanged(true);
		return;
	}

	const size_t maxScroll = mRows.size() > VisibleRows ? mRows.size() - VisibleRows : 0;
	if (IsPressed(SDL_SCANCODE_UP) && mScroll > 0)
	{
		mScroll--;
		UpdateRangeText();
	}
	if (IsPressed(SDL_SCANCODE_DOWN) && mScroll < maxScroll)
	{
		mScroll++;
		UpdateRangeText();
	}
}

void RankingScene::draw(Shader* shader) const
{
	DrawTerminalFrame(shader);

	DrawText(shader, mPrompt, 80.0f, 90.0f, Palette::Green);
	DrawText(shader, mCommand, 80.0f + mPrompt->GetWidth() + 40.0f, 90.0f, Palette::TextFaint);
	DrawText(shader, mSubtitle, 80.0f, 118.0f, Palette::Text);

	DrawText(shader, mHeaderRank, ColRank, 160.0f, Palette::Purple);
	DrawText(shader, mHeaderName, ColName, 160.0f, Palette::Purple);
	DrawText(shader, mHeaderChars, ColChars, 160.0f, Palette::Purple);
	DrawText(shader, mHeaderWhen, ColWhen, 160.0f, Palette::Purple);
	DrawRect(shader, 72.0f, 192.0f, RowWidth, 1.0f, Palette::Line);

	if (mRows.empty())
	{
		DrawText(shader, mEmpty, ColRank, RowTop + 24.0f, Palette::TextMute);
	}

	const size_t last = std::min(mScroll + VisibleRows, mRows.size());
	for (size_t i = mScroll; i < last; i++)
	{
		const Row& row = mRows[i];
		const float y = RowTop + RowStride * static_cast<float>(i - mScroll);
		const float textY = y + (RowHeight - 30.0f) * 0.5f;

		if (row.highlighted)
		{
			DrawRect(shader, RowX, y, RowWidth, RowHeight, Palette::SelectBg);
			DrawRect(shader, RowX, y, 4.0f, RowHeight, Palette::Orange);
		}

		const bool first = (i == 0);
		const Vector3 rankColor = row.highlighted ? Palette::Cyan : (first ? Palette::Orange : Palette::Text);
		const Vector3 nameColor = row.highlighted ? Palette::Orange : Palette::Text;
		const Vector3 charsColor = (row.highlighted || first) ? Palette::Lime : Palette::Text;
		const Vector3 whenColor = row.highlighted ? Palette::Orange : Palette::TextMute;

		DrawText(shader, row.rank, ColRank + 8.0f, textY, rankColor);
		DrawText(shader, row.name, ColName, textY, nameColor);
		DrawText(shader, row.chars, ColChars, textY, charsColor);
		DrawText(shader, row.when, ColWhen, textY, whenColor);
		if (row.highlighted)
		{
			DrawText(shader, mThisPlay, ColArrow, textY + 4.0f, Palette::Orange);
		}
	}

	DrawText(shader, mRange, 80.0f, 584.0f, Palette::TextFaint);
	DrawText(shader, mNote, 80.0f + mRange->GetWidth() + 40.0f, 584.0f, Palette::TextFaint);
	DrawText(shader, mFile, 80.0f + mRange->GetWidth() + 40.0f + mNote->GetWidth() + 40.0f, 584.0f, Palette::TextDark);
}

EScene RankingScene::GetScene() const
{
	return EScene::Ranking;
}
