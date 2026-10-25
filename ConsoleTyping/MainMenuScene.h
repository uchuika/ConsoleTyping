#pragma once

#include "AbstractScene.h"

// コマンドを打って操作するメインメニュー。実行した行と結果はログとして残り、
// タイピングやランキングから戻ってもそのまま見える
class MainMenuScene : public AbstractScene
{
public:
	MainMenuScene(IOnSceneChangedListener* impl, const Parameter& parameter, class Game* game);
	~MainMenuScene() override = default;

	void update(float deltaTime) override;
	void draw(class Shader* shader) const override;
	EScene GetScene() const override;

private:
	enum class LogKind
	{
		Command,
		Output,
		Hint,
		Error,
	};

	struct LogLine
	{
		LogKind kind;
		std::string text;
	};

	struct Command
	{
		const char* name;
		const char* description;
		void (MainMenuScene::*run)();
	};

	static const Command sCommands[];

	void AddLog(LogKind kind, const std::string& text);
	void RefreshTexts();

	void Execute();
	void CmdHelp();
	void CmdStart();
	void CmdRank();

	std::vector<LogLine> mLog;
	std::string mInput;

	std::vector<Text*> mLogTexts;
	Text* mInputText;
	Text* mPrompt;
};
