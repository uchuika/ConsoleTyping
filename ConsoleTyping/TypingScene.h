#pragma once

#include <string>
#include "AbstractScene.h"
#include "RomajiTyper.h"

// タイピングゲーム本体。ニックネーム入力 → 3 カウント → 60 秒のタイピング → 結果保存とランキングへ
class TypingScene : public AbstractScene
{
public:
	TypingScene(IOnSceneChangedListener* impl, const Parameter& parameter, class Game* game);
	~TypingScene() override = default;

	void update(float deltaTime) override;
	void draw(class Shader* shader) const override;
	EScene GetScene() const override;

private:
	enum class State
	{
		NameInput,
		Countdown,
		Playing,
		ConfirmQuit,
		TimeUp,
		// 保存とシーン遷移を依頼したあと。シーンが閉じられるまでの間、何もしない
		Finished,
	};

	void UpdateNameInput();
	void UpdateCountdown(unsigned long long deltaMs);
	void UpdatePlaying(unsigned long long deltaMs);
	void UpdateConfirmQuit();
	void UpdateTimeUp(unsigned long long deltaMs);

	void StartCountdown();
	void StartPlaying();
	void FinishGame();
	void AdvanceProblem();
	void UpdateRomajiTexts();
	void UpdateStatusTexts();

	void DrawNameInput(class Shader* shader) const;
	void DrawCountdown(class Shader* shader) const;
	void DrawPlaying(class Shader* shader) const;

	State mState;
	// 直前のフレームの時刻 (ms)。フレームが遅れても時間が狂わないよう実時間で数える
	unsigned long long mLastTick;
	// 現在の状態に入ってからの時間 (ms)
	unsigned long long mStateMs;
	// タイピング中の経過時間 (ms)。確認ダイアログの間は進まない
	unsigned long long mPlayMs;

	std::string mNickname;
	bool mNameRejected;

	int mChars;
	int mMisses;
	size_t mProblemIndex;
	RomajiTyper mTyper;
	bool mSaveFailed;

	// 画面に出すテキスト (変わったときだけ SetText で作り直す)
	Text* mNameTitle;
	Text* mNameField;
	Text* mNameHint;
	Text* mNameError;
	Text* mCountdownDigits[3];
	Text* mCountdownCaption;

	Text* mNickLabel;
	Text* mNickValue;
	Text* mTimeLabel;
	Text* mTimeValue;
	Text* mCharsLabel;
	Text* mCharsValue;
	Text* mRuleText;
	Text* mChip;
	Text* mPrompt;
	Text* mPromptNotice;
	Text* mLabelProblem;
	Text* mLabelRomaji;
	Text* mProblemText;
	Text* mRomajiTyped;
	Text* mRomajiRest;
	Text* mPanelHint;
	Text* mNextLabel;
	Text* mNextText;
	Text* mNextRomaji;
	Text* mProgressLabel;
	Text* mProgressValue;

	Text* mDialogKey;
	Text* mDialogTitle;
	Text* mDialogEnter;
	Text* mDialogEsc;

	Text* mTimeUpTitle;
	Text* mTimeUpScore;
	Text* mSaveWarning;

	// 値が変わったときだけテキストを作り直すための記録
	int mShownSeconds;
	int mShownChars;
	int mMonoCharWidth;
};
