#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "Singleton.h"

struct ScoreRecord
{
	int id;
	std::string name;
	// 正しく打てたローマ字の文字数
	int chars;
	// 記録した時刻 (UNIX 秒)
	int64_t playedAt;
};

// scores.json に保存するローカルのスコア表。
// 書き込みは一時ファイルに書いてから置き換えるので、途中で落ちても元のファイルは壊れない。
class ScoreBoard : public Singleton<ScoreBoard>
{
	ScoreBoard() = default;
	friend Singleton<ScoreBoard>;

public:
	// ファイルが無ければ空で始める。壊れていれば .bak に退避して空で始める。
	// 読み込み時の出来事を message に入れる (空なら問題なし)
	void Load(const std::string& path, std::string* message = nullptr);

	// 記録を追加して保存する。追加した記録の id を返す。保存に失敗しても記録はメモリに残る
	int Add(const std::string& name, int chars, int64_t playedAt, bool* saved = nullptr);

	// 文字数の多い順 (同点は先に記録した方が上)。limit 件まで
	std::vector<ScoreRecord> GetSorted(size_t limit) const;

	// 直近に Add した記録の id (無ければ -1)
	int GetLastAddedId() const { return mLastAddedId; }

	size_t Size() const { return mRecords.size(); }

private:
	bool Save() const;

	std::string mPath;
	std::vector<ScoreRecord> mRecords;
	int mNextId = 1;
	int mLastAddedId = -1;
};
