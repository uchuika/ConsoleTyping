#pragma once

#include <string>
#include <vector>
#include "Singleton.h"

struct Problem
{
	// 画面に出す問題文 (日本語)
	std::string text;
	// ひらがなの読み。ローマ字の元になる
	std::string reading;
};

// problems.txt から読み込んだ問題の一覧。出題順はファイルの順で固定する。
// 書式: 1 行 1 問、「表示文<TAB>ひらがな読み」。# で始まる行と空行は無視する。
class ProblemSet : public Singleton<ProblemSet>
{
	ProblemSet() = default;
	friend Singleton<ProblemSet>;

public:
	// 読み込み結果の警告 (不正な行など) を warnings に追加する。1 問も無ければ組み込みの問題に置き換える
	void Load(const std::string& path, std::vector<std::string>* warnings = nullptr);

	size_t Size() const { return mProblems.size(); }
	// index が末尾を越えたら先頭に戻る
	const Problem& At(size_t index) const;

private:
	std::vector<Problem> mProblems;
};
