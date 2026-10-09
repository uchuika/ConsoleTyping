#pragma once

#include <string>
#include <vector>
#include "Singleton.h"

// ng-words.txt の禁止語でニックネームを検査する。
// 書式: 1 行 1 語。# で始まる行と空行は無視する。英字は大文字小文字を区別しない部分一致。
class WordFilter : public Singleton<WordFilter>
{
	WordFilter() = default;
	friend Singleton<WordFilter>;

public:
	// ファイルが無くても禁止語 0 件として続行する。読めなければ false
	bool Load(const std::string& path);

	bool ContainsNgWord(const std::string& text) const;

private:
	std::vector<std::string> mWords;
};
