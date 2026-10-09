#pragma once

#include "AbstractScene.h"

class MainMenuScene : public AbstractScene
{
public:
	MainMenuScene(IOnSceneChangedListener* impl, const Parameter& parameter, class Game* game);
	~MainMenuScene() override = default;

	void update(float deltaTime) override;
	void draw(class Shader* shader) const override;
	EScene GetScene() const override;

private:
	struct Item
	{
		EScene scene;
		Text* title;
		Text* description;
	};

	void DrawItem(class Shader* shader, const Item& item, float y, bool selected) const;

	std::vector<Item> mItems;
	int mSelected;

	Text* mPrompt;
	Text* mSubtitle;
	Text* mSelectLabel;
	Text* mSelectHint;
	Text* mExitLabel;
	Text* mExitLine1;
	Text* mExitLine2;
};
