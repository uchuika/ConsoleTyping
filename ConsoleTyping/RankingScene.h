#pragma once

#include <string>
#include <vector>
#include "AbstractScene.h"
#include "ScoreBoard.h"

// 正しく打てたローマ字の文字数が多い順に、上位 20 件を 10 行ずつ見せる
class RankingScene : public AbstractScene
{
public:
	RankingScene(IOnSceneChangedListener* impl, const Parameter& parameter, class Game* game);
	~RankingScene() override = default;

	void update(float deltaTime) override;
	void draw(class Shader* shader) const override;
	EScene GetScene() const override;

private:
	struct Row
	{
		ScoreRecord record;
		bool highlighted;
		Text* rank;
		Text* name;
		Text* chars;
		Text* when;
	};

	void UpdateRangeText();
	std::string FormatWhen(const ScoreRecord& record, bool highlighted) const;

	std::vector<Row> mRows;
	// 画面の先頭に出す行 (0 始まり)
	size_t mScroll;

	Text* mPrompt;
	Text* mCommand;
	Text* mSubtitle;
	Text* mHeaderRank;
	Text* mHeaderName;
	Text* mHeaderChars;
	Text* mHeaderWhen;
	Text* mThisPlay;
	Text* mEmpty;
	Text* mRange;
	Text* mNote;
	Text* mFile;
};
