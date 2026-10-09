#pragma once

#include "AbstractScene.h"

// 仮のシーン。本実装までは「準備中」を表示して Esc で戻る
class TypingScene : public AbstractScene
{
public:
	TypingScene(IOnSceneChangedListener* impl, const Parameter& parameter, class Game* game);
	~TypingScene() override = default;

	void update(float deltaTime) override;
	void draw(class Shader* shader) const override;
	EScene GetScene() const override;

private:
	Text* mTitle;
	Text* mHint;
};
