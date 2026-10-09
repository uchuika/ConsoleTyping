#pragma once

#include "AbstractScene.h"

// 仮のシーン。本実装までは「準備中」を表示して Esc で戻る
class RankingScene : public AbstractScene
{
public:
	RankingScene(IOnSceneChangedListener* impl, const Parameter& parameter, class Game* game);
	~RankingScene() override = default;

	void update(float deltaTime) override;
	void draw(class Shader* shader) const override;
	EScene GetScene() const override;

private:
	Text* mTitle;
	Text* mHint;
};
