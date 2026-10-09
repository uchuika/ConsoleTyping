#pragma once

#include <string>
#include <vector>

// ひらがなの読みを、ローマ字入力の複数の打ち方で受け付ける。
// SDL や OpenGL に依存しない (テストしやすいように標準ライブラリだけで書く)。
//
// 読みに使える文字:
//   ひらがな、長音 ー (入力は - キー)、半角の英小文字・数字・-
//   半角/全角スペースは表示上の区切りで、打つ必要はない
class RomajiTyper
{
public:
	enum class Result
	{
		Miss,	// 受け付けない文字 (進まない)
		Hit,	// 正しい文字
		Done,	// 正しい文字で、問題をすべて打ち終わった
	};

	RomajiTyper();

	// 読みを設定して最初から始める。使えない文字を含むときは false (error に理由)
	bool Reset(const std::string& reading, std::string* error = nullptr);

	// 1 文字入力する (c は半角の英小文字・数字・-)
	Result Input(char c);

	bool IsDone() const;

	// 打ち終わった部分 (実際に打った経路)
	std::string GetTyped() const;
	// 残りの部分 (これから打つ標準の経路)
	std::string GetRemaining() const;

	// 読みを標準の打ち方で表したローマ字。次の問題の先読み表示などに使う
	static std::string ToRomaji(const std::string& reading);

private:
	struct Unit
	{
		// 打ち方の候補。先頭が表示に使う標準の打ち方
		std::vector<std::string> candidates;
		// このユニットの後ろに表示用のスペースを入れるか
		bool spaceAfter;
		// 「ん」のユニットか (n 1 文字で打ち終えたあとの n を受け付けるため)
		bool isN;
	};

	static bool BuildUnits(const std::string& reading, std::vector<Unit>& units, std::string* error);

	std::vector<Unit> mUnits;
	size_t mUnitIndex;
	// 現在のユニットで打った文字
	std::string mTypedInUnit;
	// 完了したユニットの打ち方 (表示用スペースを含む)
	std::string mTypedDone;
	// 「ん」を n 1 文字で打ち終えた直後。続けて n が来ても受け付ける (nn と打つ人のため)
	bool mAcceptExtraN;
};
