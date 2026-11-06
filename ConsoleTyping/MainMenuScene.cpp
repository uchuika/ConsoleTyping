#include "MainMenuScene.h"
#include <cstdio>
#include "CommandLine.h"
#include "Font.h"
#include "Keyboard.h"
#include "Palette.h"

namespace
{
	const size_t LogRows = 16;
	const size_t InputMaxLength = 32;

	const float TextX = 80.0f;
	const float TextTop = 92.0f;
	const float RowStride = 30.0f;
	const float PromptGap = 14.0f;

	const int RowFontSize = 18;

	// コマンドに使える文字を scancode から作る。使えないキーは 0
	char CommandChar(int scancode)
	{
		if (scancode == SDL_SCANCODE_SPACE)
		{
			return ' ';
		}
		return Keyboard::ToChar(scancode);
	}
}

const MainMenuScene::Command MainMenuScene::sCommands[] = {
	{ "help", u8"コマンド一覧を表示する", &MainMenuScene::CmdHelp },
	{ "start", u8"名前を入れて 3 カウント、60 秒のタイピングを始める", &MainMenuScene::CmdStart },
	{ "rank", u8"保存済みの記録を、打てた文字数の多い順に見る", &MainMenuScene::CmdRank },
};

MainMenuScene::MainMenuScene(IOnSceneChangedListener* impl, const Parameter& parameter, Game* game)
:AbstractScene(impl, parameter, game)
{
	mPrompt = AddText(u8"PS C:\\festival>", RowFontSize);
	mInputText = AddText("", RowFontSize);
	for (size_t i = 0; i < LogRows; i++)
	{
		mLogTexts.push_back(AddText("", RowFontSize));
	}

	AddLog(LogKind::Hint, u8"help と入力して Enter を押すと、コマンド一覧を表示します");
	RefreshLogTexts();
	RefreshInputText();
}

void MainMenuScene::AddLog(LogKind kind, const std::string& text)
{
	mLog.push_back({ kind, text });
	if (mLog.size() > LogRows)
	{
		mLog.pop_front();
	}
}

void MainMenuScene::RefreshLogTexts()
{
	for (size_t i = 0; i < mLogTexts.size(); i++)
	{
		mLogTexts[i]->SetText(i < mLog.size() ? mLog[i].text : std::string());
	}
}

void MainMenuScene::RefreshInputText()
{
	mInputText->SetText(mInput);
}

void MainMenuScene::update(float deltaTime)
{
	Keyboard* keyboard = Keyboard::getIns();
	const bool alt = keyboard->GetPressingCount(SDL_SCANCODE_LALT) > 0
		|| keyboard->GetPressingCount(SDL_SCANCODE_RALT) > 0;

	bool changed = false;
	for (int scancode : keyboard->GetKeyDowns())
	{
		if (scancode == SDL_SCANCODE_ESCAPE)
		{
			mInput.clear();
			changed = true;
			continue;
		}
		if (scancode == SDL_SCANCODE_RETURN)
		{
			// Alt+Enter は全画面の切り替えなので実行にしない
			if (alt)
			{
				continue;
			}
			Execute();
			RefreshLogTexts();
			changed = true;
			break;
		}
		if (scancode == SDL_SCANCODE_BACKSPACE)
		{
			if (!mInput.empty())
			{
				mInput.pop_back();
				changed = true;
			}
			continue;
		}

		const char c = CommandChar(scancode);
		if (c != 0 && mInput.size() < InputMaxLength)
		{
			mInput.push_back(c);
			changed = true;
		}
	}

	if (changed)
	{
		RefreshInputText();
	}
}

void MainMenuScene::Execute()
{
	const std::string line = mInput;
	mInput.clear();
	AddLog(LogKind::Command, line);

	const std::string name = CommandLine::FirstWord(line);
	if (name.empty())
	{
		return;
	}

	for (const Command& command : sCommands)
	{
		if (name == command.name)
		{
			(this->*command.run)();
			return;
		}
	}
	AddLog(LogKind::Error, "'" + name + u8"' は認識されないコマンドです。help でコマンド一覧を表示します");
}

void MainMenuScene::CmdHelp()
{
	for (const Command& command : sCommands)
	{
		char name[16];
		std::snprintf(name, sizeof(name), "%-8s", command.name);
		AddLog(LogKind::Output, std::string("  ") + name + command.description);
	}
}

void MainMenuScene::CmdStart()
{
	mListener->onSceneChanged(EScene::Typing, Parameter(), false);
}

void MainMenuScene::CmdRank()
{
	mListener->onSceneChanged(EScene::Ranking, Parameter(), false);
}

void MainMenuScene::draw(Shader* shader) const
{
	DrawTerminalFrame(shader);

	const float inputX = TextX + mPrompt->GetWidth() + PromptGap;
	const size_t shown = mLog.size();
	for (size_t i = 0; i < shown; i++)
	{
		const LogLine& line = mLog[i];
		const float y = TextTop + RowStride * static_cast<float>(i);
		switch (line.kind)
		{
		case LogKind::Command:
			DrawText(shader, mPrompt, TextX, y, Palette::Green);
			DrawText(shader, mLogTexts[i], inputX, y, Palette::Text);
			break;
		case LogKind::Output:
			DrawText(shader, mLogTexts[i], TextX, y, Palette::TextMute);
			break;
		case LogKind::Hint:
			DrawText(shader, mLogTexts[i], TextX, y, Palette::TextFaint);
			break;
		case LogKind::Error:
			DrawText(shader, mLogTexts[i], TextX, y, Palette::Red);
			break;
		}
	}

	// 入力中の行。文字の後ろの点滅しないカーソル
	const float inputY = TextTop + RowStride * static_cast<float>(shown);
	DrawText(shader, mPrompt, TextX, inputY, Palette::Green);
	DrawText(shader, mInputText, inputX, inputY, Palette::TextBright);
	const float cursorX = inputX + mFont->GetTextWidth(mInput, RowFontSize) + 2.0f;
	DrawRect(shader, cursorX, inputY + 1.0f, 3.0f, mPrompt->GetDrawHeight() - 2.0f, Palette::Purple);
}

EScene MainMenuScene::GetScene() const
{
	return EScene::MainMenu;
}
