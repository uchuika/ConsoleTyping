#include "TypingScene.h"
#include <cstdio>
#include <ctime>
#include "Font.h"
#include "Game.h"
#include "Keyboard.h"
#include "Palette.h"
#include "ProblemSet.h"
#include "ScoreBoard.h"
#include "WordFilter.h"

namespace
{
	const unsigned long long PlayLimitMs = 60 * 1000;
	const unsigned long long CountdownStepMs = 1000;
	const unsigned long long TimeUpShowMs = 2500;
	const size_t NicknameMaxLength = 12;
	const char* DefaultNickname = "Player";

	const int RomajiSize = 32;

	// ニックネームに使える文字を scancode から作る。使えないキーは 0
	char NicknameChar(int scancode, bool shift)
	{
		if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z)
		{
			return static_cast<char>((shift ? 'A' : 'a') + (scancode - SDL_SCANCODE_A));
		}
		if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_9)
		{
			return static_cast<char>('1' + (scancode - SDL_SCANCODE_1));
		}
		if (scancode == SDL_SCANCODE_0)
		{
			return '0';
		}
		if (scancode == SDL_SCANCODE_MINUS)
		{
			return shift ? '_' : '-';
		}
		// 日本語配列の _ のキー
		if (scancode == SDL_SCANCODE_INTERNATIONAL1)
		{
			return shift ? '_' : 0;
		}
		return 0;
	}
}

TypingScene::TypingScene(IOnSceneChangedListener* impl, const Parameter& parameter, Game* game)
:AbstractScene(impl, parameter, game)
,mState(State::NameInput)
,mLastTick(SDL_GetTicks64())
,mStateMs(0)
,mPlayMs(0)
,mNickname(DefaultNickname)
,mNameRejected(false)
,mChars(0)
,mMisses(0)
,mProblemIndex(0)
,mSaveFailed(false)
,mShownSeconds(-1)
,mShownChars(-1)
{
	SetFrameInfo(u8"typing  ·  60s", {
		{ u8"文字", u8"入力" },
		{ u8"Esc", u8"やめる？" },
		{ u8"Alt+Enter", u8"全画面 / ウィンドウ" },
	});

	mMonoCharWidth = mFont->GetTextWidth("M", RomajiSize);

	// ニックネーム入力
	mNameTitle = AddText(u8"ニックネームを入力してください", 26);
	mNameField = AddText(mNickname, 30);
	mNameHint = AddText(u8"英数字・_・- が使えます (12 文字まで)    Enter で決定    Esc でメニューへ", 15);
	mNameError = AddText(u8"使えない言葉が含まれています。別の名前にしてください", 16);

	// カウントダウン
	mCountdownDigits[0] = AddText("3", 120, true);
	mCountdownDigits[1] = AddText("2", 120, true);
	mCountdownDigits[2] = AddText("1", 120, true);
	mCountdownCaption = AddText(u8"準備はいい？ 画面の文を、そのとおりに打とう", 20);

	// タイピング中
	mNickLabel = AddText("nick", 13);
	mNickValue = AddText(mNickname, 24);
	mTimeLabel = AddText("time", 13);
	mTimeValue = AddText("01:00", 26);
	mCharsLabel = AddText("chars", 13);
	mCharsValue = AddText("0", 26);
	mRuleText = AddText(u8"正しいローマ字 1 文字 = 1", 16);
	mChip = AddText(u8"3 カウント済", 16);
	mPrompt = AddText("PS C:\\Users\\" + mNickname + ">", 18);
	mPromptNotice = AddText(u8"下の文を打ってください", 22);
	mLabelProblem = AddText(u8"01  問題文", 13);
	mLabelRomaji = AddText(u8"02  入力するローマ字", 13);
	mProblemText = AddText("", 32);
	mRomajiTyped = AddText("", RomajiSize);
	mRomajiRest = AddText("", RomajiSize);
	mPanelHint = AddText(u8"誤キーでは進まない。Shift やキーリピートは無視。打ち切ると次の問題へ。", 14);
	mNextLabel = AddText("next", 13);
	mNextText = AddText("", 20);
	mNextRomaji = AddText("", 15);
	mProgressLabel = AddText("progress", 13);
	mProgressValue = AddText("0s", 15);

	mDialogKey = AddText("Esc", 14);
	mDialogTitle = AddText(u8"やめる？", 26);
	mDialogEnter = AddText(u8"Enter でメニューへ", 18);
	mDialogEsc = AddText(u8"Esc で続行", 18);

	mTimeUpTitle = AddText("TIME UP", 64, true);
	mTimeUpScore = AddText("", 34);
	mSaveWarning = AddText(u8"スコアを保存できませんでした", 16);
}

void TypingScene::update(float deltaTime)
{
	const unsigned long long now = SDL_GetTicks64();
	const unsigned long long deltaMs = now - mLastTick;
	mLastTick = now;
	mStateMs += deltaMs;

	switch (mState)
	{
	case State::NameInput:
		UpdateNameInput();
		break;
	case State::Countdown:
		UpdateCountdown(deltaMs);
		break;
	case State::Playing:
		UpdatePlaying(deltaMs);
		break;
	case State::ConfirmQuit:
		UpdateConfirmQuit();
		break;
	case State::TimeUp:
		UpdateTimeUp(deltaMs);
		break;
	case State::Finished:
		break;
	}
}

void TypingScene::UpdateNameInput()
{
	Keyboard* keyboard = Keyboard::getIns();
	const bool shift = keyboard->GetPressingCount(SDL_SCANCODE_LSHIFT) > 0
		|| keyboard->GetPressingCount(SDL_SCANCODE_RSHIFT) > 0;
	const bool alt = keyboard->GetPressingCount(SDL_SCANCODE_LALT) > 0
		|| keyboard->GetPressingCount(SDL_SCANCODE_RALT) > 0;

	bool changed = false;
	for (int scancode : keyboard->GetKeyDowns())
	{
		if (scancode == SDL_SCANCODE_ESCAPE)
		{
			mListener->onSceneChanged(true);
			return;
		}
		if (scancode == SDL_SCANCODE_RETURN)
		{
			// Alt+Enter は全画面の切り替えなので決定にしない
			if (alt)
			{
				continue;
			}
			if (mNickname.empty())
			{
				mNickname = DefaultNickname;
			}
			if (WordFilter::getIns()->ContainsNgWord(mNickname))
			{
				mNameRejected = true;
				mNameField->SetText(mNickname);
				return;
			}
			StartCountdown();
			return;
		}
		if (scancode == SDL_SCANCODE_BACKSPACE)
		{
			if (!mNickname.empty())
			{
				mNickname.pop_back();
				changed = true;
			}
			continue;
		}

		const char c = NicknameChar(scancode, shift);
		if (c != 0 && mNickname.size() < NicknameMaxLength)
		{
			mNickname.push_back(c);
			changed = true;
		}
	}

	if (changed)
	{
		mNameRejected = false;
		mNameField->SetText(mNickname);
	}
}

void TypingScene::StartCountdown()
{
	mNickValue->SetText(mNickname);
	mPrompt->SetText("PS C:\\Users\\" + mNickname + ">");
	mState = State::Countdown;
	mStateMs = 0;
	mChip->SetText(u8"カウント中");
}

void TypingScene::UpdateCountdown(unsigned long long deltaMs)
{
	if (mStateMs >= CountdownStepMs * 3)
	{
		StartPlaying();
	}
}

void TypingScene::StartPlaying()
{
	mState = State::Playing;
	mStateMs = 0;
	mPlayMs = 0;
	mChars = 0;
	mMisses = 0;
	mProblemIndex = static_cast<size_t>(-1);
	mShownSeconds = -1;
	mShownChars = -1;
	mChip->SetText(u8"3 カウント済");
	AdvanceProblem();
	UpdateStatusTexts();
}

void TypingScene::AdvanceProblem()
{
	mProblemIndex++;
	const Problem& problem = ProblemSet::getIns()->At(mProblemIndex);
	const Problem& next = ProblemSet::getIns()->At(mProblemIndex + 1);

	mTyper.Reset(problem.reading);
	mProblemText->SetText(problem.text);
	mNextText->SetText(next.text);
	mNextRomaji->SetText(RomajiTyper::ToRomaji(next.reading));
	UpdateRomajiTexts();
}

void TypingScene::UpdateRomajiTexts()
{
	mRomajiTyped->SetText(mTyper.GetTyped());
	mRomajiRest->SetText(mTyper.GetRemaining());
}

void TypingScene::UpdateStatusTexts()
{
	const unsigned long long remainMs = mPlayMs >= PlayLimitMs ? 0 : PlayLimitMs - mPlayMs;
	const int remainSeconds = static_cast<int>((remainMs + 999) / 1000);
	if (remainSeconds != mShownSeconds)
	{
		mShownSeconds = remainSeconds;
		char buffer[16];
		std::snprintf(buffer, sizeof(buffer), "%02d:%02d", remainSeconds / 60, remainSeconds % 60);
		mTimeValue->SetText(buffer);
		const int elapsed = 60 - remainSeconds;
		mProgressValue->SetText(std::to_string(elapsed) + "s");
	}
	if (mChars != mShownChars)
	{
		mShownChars = mChars;
		mCharsValue->SetText(std::to_string(mChars));
	}
}

void TypingScene::UpdatePlaying(unsigned long long deltaMs)
{
	mPlayMs += deltaMs;
	if (mPlayMs >= PlayLimitMs)
	{
		mPlayMs = PlayLimitMs;
		UpdateStatusTexts();
		FinishGame();
		return;
	}

	bool romajiChanged = false;
	for (int scancode : Keyboard::getIns()->GetKeyDowns())
	{
		if (scancode == SDL_SCANCODE_ESCAPE)
		{
			mState = State::ConfirmQuit;
			mStateMs = 0;
			break;
		}

		// 修飾キーなど対象外のキーは 0 になって無視される
		const char c = Keyboard::ToChar(scancode);
		if (c == 0)
		{
			continue;
		}

		const RomajiTyper::Result result = mTyper.Input(c);
		if (result == RomajiTyper::Result::Miss)
		{
			mMisses++;
			continue;
		}

		mChars++;
		romajiChanged = true;
		if (result == RomajiTyper::Result::Done)
		{
			AdvanceProblem();
			romajiChanged = false;
		}
	}

	if (romajiChanged)
	{
		UpdateRomajiTexts();
	}
	UpdateStatusTexts();
}

void TypingScene::UpdateConfirmQuit()
{
	Keyboard* keyboard = Keyboard::getIns();
	const bool alt = keyboard->GetPressingCount(SDL_SCANCODE_LALT) > 0
		|| keyboard->GetPressingCount(SDL_SCANCODE_RALT) > 0;
	for (int scancode : keyboard->GetKeyDowns())
	{
		if (scancode == SDL_SCANCODE_ESCAPE)
		{
			mState = State::Playing;
			mStateMs = 0;
			return;
		}
		if (scancode == SDL_SCANCODE_RETURN && !alt)
		{
			mListener->onSceneChanged(true);
			return;
		}
	}
}

void TypingScene::FinishGame()
{
	mState = State::TimeUp;
	mStateMs = 0;
	mTimeUpScore->SetText(std::to_string(mChars) + u8" 文字");
}

void TypingScene::UpdateTimeUp(unsigned long long deltaMs)
{
	if (mStateMs < TimeUpShowMs)
	{
		return;
	}

	bool saved = false;
	ScoreBoard::getIns()->Add(mNickname, mChars, static_cast<int64_t>(std::time(nullptr)), &saved);
	mSaveFailed = !saved;

	// タイピング画面を閉じて、今回のプレイを強調したランキングを開く。Esc でメニューへ戻れる
	Parameter parameter;
	parameter.set("highlight", 1);
	mListener->onSceneChanged(true);
	mListener->onSceneChanged(EScene::Ranking, parameter, false);

	// シーンが閉じられるまでの間に二重に保存しないよう、止まる状態に移す
	mState = State::Finished;
}

void TypingScene::draw(Shader* shader) const
{
	DrawTerminalFrame(shader);

	switch (mState)
	{
	case State::NameInput:
		DrawNameInput(shader);
		break;
	case State::Countdown:
		DrawCountdown(shader);
		break;
	case State::Playing:
	case State::ConfirmQuit:
	case State::TimeUp:
	case State::Finished:
		DrawPlaying(shader);
		break;
	}
}

void TypingScene::DrawNameInput(Shader* shader) const
{
	DrawText(shader, mNameTitle, 80.0f, 120.0f, Palette::TextBright);

	DrawRect(shader, 80.0f, 184.0f, 560.0f, 64.0f, Palette::SelectBg);
	DrawBorder(shader, 80.0f, 184.0f, 560.0f, 64.0f, mNameRejected ? Palette::Red : Palette::Purple);
	DrawText(shader, mNameField, 104.0f, 196.0f, Palette::Orange);

	// 文字の後ろの点滅しないカーソル
	const float cursorX = 104.0f + mFont->GetTextWidth(mNickname, 30) + 2.0f;
	DrawRect(shader, cursorX, 196.0f, 3.0f, 40.0f, Palette::Purple);

	DrawText(shader, mNameHint, 80.0f, 268.0f, Palette::TextMute);
	if (mNameRejected)
	{
		DrawText(shader, mNameError, 80.0f, 300.0f, Palette::Red);
	}
}

void TypingScene::DrawCountdown(Shader* shader) const
{
	int step = static_cast<int>(mStateMs / CountdownStepMs);
	if (step > 2)
	{
		step = 2;
	}
	const Text* digit = mCountdownDigits[step];
	DrawText(shader, digit, 640.0f - digit->GetWidth() * 0.5f, 250.0f, Palette::Orange);
	DrawText(shader, mCountdownCaption, 640.0f - mCountdownCaption->GetWidth() * 0.5f, 430.0f, Palette::TextMute);
}

void TypingScene::DrawPlaying(Shader* shader) const
{
	// 情報ボックス
	DrawRect(shader, 56.0f, 84.0f, 1168.0f, 48.0f, Palette::SelectBg, 0.45f);
	DrawBorder(shader, 56.0f, 84.0f, 1168.0f, 48.0f, Palette::Line);
	DrawText(shader, mNickLabel, 76.0f, 100.0f, Palette::TextFaint);
	DrawText(shader, mNickValue, 124.0f, 90.0f, Palette::Orange);
	DrawText(shader, mTimeLabel, 330.0f, 100.0f, Palette::TextFaint);
	DrawText(shader, mTimeValue, 372.0f, 90.0f, Palette::Cyan);
	DrawText(shader, mCharsLabel, 520.0f, 100.0f, Palette::TextFaint);
	DrawText(shader, mCharsValue, 580.0f, 90.0f, Palette::Lime);
	DrawText(shader, mRuleText, 650.0f, 98.0f, Palette::TextMute);
	DrawText(shader, mChip, 1204.0f - mChip->GetWidth(), 98.0f, Palette::TextDark);

	// プロンプト
	DrawText(shader, mPrompt, 72.0f, 150.0f, Palette::Green);
	DrawText(shader, mPromptNotice, 72.0f + mPrompt->GetWidth() + 40.0f, 148.0f, Palette::Orange);

	// 問題パネル
	DrawBorder(shader, 56.0f, 188.0f, 1168.0f, 190.0f, Palette::Line);
	DrawRect(shader, 56.0f, 188.0f, 4.0f, 190.0f, Palette::Blue);
	DrawText(shader, mLabelProblem, 84.0f, 200.0f, Palette::Blue);
	DrawText(shader, mProblemText, 84.0f, 222.0f, Palette::TextBright);
	DrawText(shader, mLabelRomaji, 84.0f, 282.0f, Palette::Purple);

	// ローマ字: 打った部分、カーソル位置の 1 文字、残り
	const float romajiX = 84.0f;
	const float romajiY = 302.0f;
	const float typedWidth = static_cast<float>(mFont->GetTextWidth(mTyper.GetTyped(), RomajiSize));
	if (!mTyper.IsDone())
	{
		DrawRect(shader, romajiX + typedWidth, romajiY - 2.0f, static_cast<float>(mMonoCharWidth), 44.0f, Palette::CursorBg);
	}
	DrawText(shader, mRomajiTyped, romajiX, romajiY, Palette::Lime);
	DrawText(shader, mRomajiRest, romajiX + typedWidth, romajiY, Palette::TextDim);
	DrawText(shader, mPanelHint, 84.0f, 352.0f, Palette::TextFaint);

	// 次の問題
	DrawText(shader, mNextLabel, 64.0f, 394.0f, Palette::TextDark);
	DrawText(shader, mNextText, 128.0f, 386.0f, Palette::TextDark);
	DrawText(shader, mNextRomaji, 128.0f, 416.0f, Palette::TextDark);

	// 進み具合
	DrawText(shader, mProgressLabel, 64.0f, 462.0f, Palette::TextFaint);
	const float barX = 150.0f;
	const float barW = 950.0f;
	DrawRect(shader, barX, 468.0f, barW, 8.0f, Palette::SelectBg);
	float ratio = static_cast<float>(mPlayMs) / static_cast<float>(PlayLimitMs);
	ratio = ratio < 0.0f ? 0.0f : (ratio > 1.0f ? 1.0f : ratio);
	DrawRect(shader, barX, 468.0f, barW * ratio, 8.0f, Palette::Green);
	DrawText(shader, mProgressValue, barX + barW + 20.0f, 460.0f, Palette::TextMute);

	if (mState == State::ConfirmQuit)
	{
		DrawRect(shader, 832.0f, 508.0f, 380.0f, 88.0f, Palette::Panel);
		DrawBorder(shader, 832.0f, 508.0f, 380.0f, 88.0f, Palette::OrangeLine);
		DrawText(shader, mDialogKey, 852.0f, 528.0f, Palette::Orange);
		DrawText(shader, mDialogTitle, 900.0f, 518.0f, Palette::TextBright);
		DrawText(shader, mDialogEnter, 852.0f, 558.0f, Palette::TextMute);
		DrawText(shader, mDialogEsc, 1052.0f, 558.0f, Palette::TextMute);
	}

	if (mState == State::TimeUp || mState == State::Finished)
	{
		DrawRect(shader, 56.0f, 150.0f, 1168.0f, 340.0f, Palette::Panel);
		DrawBorder(shader, 56.0f, 150.0f, 1168.0f, 340.0f, Palette::OrangeLine);
		DrawText(shader, mTimeUpTitle, 640.0f - mTimeUpTitle->GetWidth() * 0.5f, 210.0f, Palette::Orange);
		DrawText(shader, mTimeUpScore, 640.0f - mTimeUpScore->GetWidth() * 0.5f, 330.0f, Palette::Lime);
	}
}

EScene TypingScene::GetScene() const
{
	return EScene::Typing;
}
