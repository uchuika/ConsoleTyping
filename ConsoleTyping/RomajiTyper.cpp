#include "RomajiTyper.h"
#include <cstdint>
#include <unordered_map>

namespace
{
	typedef std::vector<std::string> Candidates;
	typedef std::unordered_map<std::string, Candidates> Table;

	// UTF-8 の文字列を 1 文字ずつに分ける。不正なバイト列のときは false
	bool SplitUtf8(const std::string& text, std::vector<std::string>& out)
	{
		size_t i = 0;
		while (i < text.size())
		{
			const unsigned char lead = static_cast<unsigned char>(text[i]);
			size_t len = 1;
			if (lead >= 0xF0)
			{
				len = 4;
			}
			else if (lead >= 0xE0)
			{
				len = 3;
			}
			else if (lead >= 0xC0)
			{
				len = 2;
			}
			else if (lead >= 0x80)
			{
				return false;
			}
			if (i + len > text.size())
			{
				return false;
			}
			out.push_back(text.substr(i, len));
			i += len;
		}
		return true;
	}

	// 1 文字のひらがなの打ち方 (先頭が標準。表示は読みやすいヘボン式寄り)
	const Table& BasicTable()
	{
		static const Table table = {
			{ u8"あ", { "a" } }, { u8"い", { "i" } }, { u8"う", { "u", "wu" } },
			{ u8"え", { "e" } }, { u8"お", { "o" } },
			{ u8"か", { "ka", "ca" } }, { u8"き", { "ki" } }, { u8"く", { "ku", "cu", "qu" } },
			{ u8"け", { "ke" } }, { u8"こ", { "ko", "co" } },
			{ u8"さ", { "sa" } }, { u8"し", { "shi", "si", "ci" } }, { u8"す", { "su" } },
			{ u8"せ", { "se", "ce" } }, { u8"そ", { "so" } },
			{ u8"た", { "ta" } }, { u8"ち", { "chi", "ti" } }, { u8"つ", { "tsu", "tu" } },
			{ u8"て", { "te" } }, { u8"と", { "to" } },
			{ u8"な", { "na" } }, { u8"に", { "ni" } }, { u8"ぬ", { "nu" } },
			{ u8"ね", { "ne" } }, { u8"の", { "no" } },
			{ u8"は", { "ha" } }, { u8"ひ", { "hi" } }, { u8"ふ", { "fu", "hu" } },
			{ u8"へ", { "he" } }, { u8"ほ", { "ho" } },
			{ u8"ま", { "ma" } }, { u8"み", { "mi" } }, { u8"む", { "mu" } },
			{ u8"め", { "me" } }, { u8"も", { "mo" } },
			{ u8"や", { "ya" } }, { u8"ゆ", { "yu" } }, { u8"よ", { "yo" } },
			{ u8"ら", { "ra" } }, { u8"り", { "ri" } }, { u8"る", { "ru" } },
			{ u8"れ", { "re" } }, { u8"ろ", { "ro" } },
			{ u8"わ", { "wa" } }, { u8"を", { "wo" } },
			{ u8"が", { "ga" } }, { u8"ぎ", { "gi" } }, { u8"ぐ", { "gu" } },
			{ u8"げ", { "ge" } }, { u8"ご", { "go" } },
			{ u8"ざ", { "za" } }, { u8"じ", { "ji", "zi" } }, { u8"ず", { "zu" } },
			{ u8"ぜ", { "ze" } }, { u8"ぞ", { "zo" } },
			{ u8"だ", { "da" } }, { u8"ぢ", { "di" } }, { u8"づ", { "du" } },
			{ u8"で", { "de" } }, { u8"ど", { "do" } },
			{ u8"ば", { "ba" } }, { u8"び", { "bi" } }, { u8"ぶ", { "bu" } },
			{ u8"べ", { "be" } }, { u8"ぼ", { "bo" } },
			{ u8"ぱ", { "pa" } }, { u8"ぴ", { "pi" } }, { u8"ぷ", { "pu" } },
			{ u8"ぺ", { "pe" } }, { u8"ぽ", { "po" } },
		};
		return table;
	}

	// 小さい仮名 (単独で打つときと、直前の仮名に付くときの両方で使う)
	const Table& SmallTable()
	{
		static const Table table = {
			{ u8"ぁ", { "xa", "la" } }, { u8"ぃ", { "xi", "li" } }, { u8"ぅ", { "xu", "lu" } },
			{ u8"ぇ", { "xe", "le" } }, { u8"ぉ", { "xo", "lo" } },
			{ u8"ゃ", { "xya", "lya" } }, { u8"ゅ", { "xyu", "lyu" } }, { u8"ょ", { "xyo", "lyo" } },
			{ u8"ゎ", { "xwa", "lwa" } },
		};
		return table;
	}

	// 拗音など 2 文字で 1 音になる打ち方
	const Table& ComboTable()
	{
		static const Table table = {
			{ u8"きゃ", { "kya" } }, { u8"きゅ", { "kyu" } }, { u8"きょ", { "kyo" } },
			{ u8"ぎゃ", { "gya" } }, { u8"ぎゅ", { "gyu" } }, { u8"ぎょ", { "gyo" } },
			{ u8"しゃ", { "sha", "sya" } }, { u8"しゅ", { "shu", "syu" } }, { u8"しょ", { "sho", "syo" } },
			{ u8"しぇ", { "she", "sye" } },
			{ u8"じゃ", { "ja", "zya", "jya" } }, { u8"じゅ", { "ju", "zyu", "jyu" } },
			{ u8"じょ", { "jo", "zyo", "jyo" } }, { u8"じぇ", { "je", "zye" } },
			{ u8"ちゃ", { "cha", "tya", "cya" } }, { u8"ちゅ", { "chu", "tyu", "cyu" } },
			{ u8"ちょ", { "cho", "tyo", "cyo" } }, { u8"ちぇ", { "che", "tye" } },
			{ u8"ぢゃ", { "dya" } }, { u8"ぢゅ", { "dyu" } }, { u8"ぢょ", { "dyo" } },
			{ u8"にゃ", { "nya" } }, { u8"にゅ", { "nyu" } }, { u8"にょ", { "nyo" } },
			{ u8"ひゃ", { "hya" } }, { u8"ひゅ", { "hyu" } }, { u8"ひょ", { "hyo" } },
			{ u8"びゃ", { "bya" } }, { u8"びゅ", { "byu" } }, { u8"びょ", { "byo" } },
			{ u8"ぴゃ", { "pya" } }, { u8"ぴゅ", { "pyu" } }, { u8"ぴょ", { "pyo" } },
			{ u8"みゃ", { "mya" } }, { u8"みゅ", { "myu" } }, { u8"みょ", { "myo" } },
			{ u8"りゃ", { "rya" } }, { u8"りゅ", { "ryu" } }, { u8"りょ", { "ryo" } },
			{ u8"ふぁ", { "fa", "fwa" } }, { u8"ふぃ", { "fi", "fyi" } },
			{ u8"ふぇ", { "fe", "fye" } }, { u8"ふぉ", { "fo", "fwo" } },
			{ u8"てぃ", { "thi" } }, { u8"てゅ", { "thu" } },
			{ u8"でぃ", { "dhi" } }, { u8"でゅ", { "dhu" } },
			{ u8"うぃ", { "wi" } }, { u8"うぇ", { "we" } },
		};
		return table;
	}

	bool IsSmallKana(const std::string& ch)
	{
		return SmallTable().count(ch) > 0;
	}

	// 先頭の文字が「っ」で二重にできる子音か (母音・n・記号は不可)
	bool CanDouble(const std::string& candidate)
	{
		if (candidate.empty())
		{
			return false;
		}
		const char c = candidate[0];
		if (c < 'a' || c > 'z')
		{
			return false;
		}
		return c != 'a' && c != 'i' && c != 'u' && c != 'e' && c != 'o' && c != 'n';
	}

	// 「ん」を n 1 文字で打てる条件: 次の音が母音・n・y で始まらない
	bool NextAllowsSingleN(const Candidates& next)
	{
		for (const std::string& c : next)
		{
			if (c.empty())
			{
				return false;
			}
			const char first = c[0];
			if (first == 'a' || first == 'i' || first == 'u' || first == 'e' || first == 'o'
				|| first == 'n' || first == 'y')
			{
				return false;
			}
		}
		return true;
	}

	struct Token
	{
		std::string kana;
		bool spaceAfter;
	};
}

RomajiTyper::RomajiTyper()
:mUnitIndex(0)
,mAcceptExtraN(false)
{
}

bool RomajiTyper::BuildUnits(const std::string& reading, std::vector<Unit>& units, std::string* error)
{
	auto fail = [error](const std::string& message)
	{
		if (error)
		{
			*error = message;
		}
		return false;
	};

	std::vector<std::string> chars;
	if (!SplitUtf8(reading, chars))
	{
		return fail("invalid UTF-8");
	}

	// 1. 文字を音の単位 (トークン) にまとめる
	std::vector<Token> tokens;
	for (const std::string& ch : chars)
	{
		if (ch == " " || ch == u8"　")
		{
			if (!tokens.empty())
			{
				tokens.back().spaceAfter = true;
			}
			continue;
		}

		// 直前の仮名と合わせて 1 音になるか (きゃ、ふぁ など)
		if (IsSmallKana(ch) && !tokens.empty() && !tokens.back().spaceAfter)
		{
			const std::string combined = tokens.back().kana + ch;
			const std::string& prev = tokens.back().kana;
			if (ComboTable().count(combined) > 0
				|| (BasicTable().count(prev) > 0 && ch != u8"ゎ"))
			{
				tokens.back().kana = combined;
				continue;
			}
		}
		tokens.push_back({ ch, false });
	}
	if (tokens.empty())
	{
		return fail("empty reading");
	}

	// 2. 後ろのトークンから打ち方の候補を作る (「ん」と「っ」は次の音で決まるため)
	std::vector<Unit> reversed;
	for (size_t i = tokens.size(); i-- > 0;)
	{
		const Token& token = tokens[i];
		const Unit* next = reversed.empty() ? nullptr : &reversed.back();
		Unit unit;
		unit.spaceAfter = token.spaceAfter;
		unit.isN = false;

		if (token.kana == u8"ん")
		{
			if (next && NextAllowsSingleN(next->candidates))
			{
				unit.candidates.push_back("n");
			}
			unit.candidates.push_back("nn");
			unit.candidates.push_back("xn");
			unit.isN = true;
		}
		else if (token.kana == u8"っ")
		{
			const char* standalone[] = { "ltu", "xtu", "ltsu", "xtsu" };
			if (next && !next->isN && !token.spaceAfter)
			{
				// 次の音と合わせて 1 ユニットにする (kka のように子音を重ねる打ち方を含む)
				for (const std::string& cand : next->candidates)
				{
					if (cand.compare(0, 2, "ch") == 0)
					{
						unit.candidates.push_back("t" + cand);
					}
					if (CanDouble(cand))
					{
						unit.candidates.push_back(cand.substr(0, 1) + cand);
					}
				}
				for (const char* prefix : standalone)
				{
					for (const std::string& cand : next->candidates)
					{
						unit.candidates.push_back(prefix + cand);
					}
				}
				unit.spaceAfter = next->spaceAfter;
				unit.isN = false;
				reversed.pop_back();
			}
			else
			{
				for (const char* prefix : standalone)
				{
					unit.candidates.push_back(prefix);
				}
			}
		}
		else if (token.kana == u8"ー" || token.kana == "-")
		{
			unit.candidates.push_back("-");
		}
		else if (token.kana.size() == 1
			&& ((token.kana[0] >= 'a' && token.kana[0] <= 'z') || (token.kana[0] >= '0' && token.kana[0] <= '9')))
		{
			unit.candidates.push_back(token.kana);
		}
		else if (BasicTable().count(token.kana) > 0)
		{
			unit.candidates = BasicTable().at(token.kana);
		}
		else if (SmallTable().count(token.kana) > 0)
		{
			unit.candidates = SmallTable().at(token.kana);
		}
		else
		{
			// 2 文字の組み合わせ (きゃ、ふぁ、きぃ など)
			std::vector<std::string> pair;
			if (!SplitUtf8(token.kana, pair) || pair.size() != 2)
			{
				return fail("unsupported character: " + token.kana);
			}
			auto explicitIter = ComboTable().find(token.kana);
			if (explicitIter != ComboTable().end())
			{
				unit.candidates = explicitIter->second;
			}
			const Candidates& base = BasicTable().at(pair[0]);
			for (const std::string& small : SmallTable().at(pair[1]))
			{
				unit.candidates.push_back(base[0] + small);
			}
		}
		reversed.push_back(unit);
	}

	units.assign(reversed.rbegin(), reversed.rend());
	return true;
}

bool RomajiTyper::Reset(const std::string& reading, std::string* error)
{
	mUnits.clear();
	mUnitIndex = 0;
	mTypedInUnit.clear();
	mTypedDone.clear();
	mAcceptExtraN = false;
	return BuildUnits(reading, mUnits, error);
}

RomajiTyper::Result RomajiTyper::Input(char c)
{
	if (IsDone())
	{
		return Result::Miss;
	}

	if (mAcceptExtraN && c == 'n')
	{
		mAcceptExtraN = false;
		return Result::Hit;
	}
	mAcceptExtraN = false;

	const Unit& unit = mUnits[mUnitIndex];
	const std::string typed = mTypedInUnit + c;

	bool prefixFound = false;
	bool complete = false;
	for (const std::string& cand : unit.candidates)
	{
		if (cand.size() >= typed.size() && cand.compare(0, typed.size(), typed) == 0)
		{
			prefixFound = true;
			if (cand.size() == typed.size())
			{
				complete = true;
			}
		}
	}
	if (!prefixFound)
	{
		return Result::Miss;
	}

	if (!complete)
	{
		mTypedInUnit = typed;
		return Result::Hit;
	}

	// ユニット完了。n と nn のように前方一致する候補があっても、打ち終えた方を採用する
	mTypedDone += typed;
	if (unit.spaceAfter)
	{
		mTypedDone += ' ';
	}
	mTypedInUnit.clear();
	mAcceptExtraN = unit.isN && typed == "n";
	mUnitIndex++;
	return IsDone() ? Result::Done : Result::Hit;
}

bool RomajiTyper::IsDone() const
{
	return mUnitIndex >= mUnits.size();
}

std::string RomajiTyper::GetTyped() const
{
	return mTypedDone + mTypedInUnit;
}

std::string RomajiTyper::GetRemaining() const
{
	std::string result;
	for (size_t i = mUnitIndex; i < mUnits.size(); i++)
	{
		const Unit& unit = mUnits[i];
		if (i == mUnitIndex)
		{
			// 打ちかけの経路に合う最初の候補の残り
			for (const std::string& cand : unit.candidates)
			{
				if (cand.size() >= mTypedInUnit.size() && cand.compare(0, mTypedInUnit.size(), mTypedInUnit) == 0)
				{
					result += cand.substr(mTypedInUnit.size());
					break;
				}
			}
		}
		else
		{
			result += unit.candidates[0];
		}
		if (unit.spaceAfter && i + 1 < mUnits.size())
		{
			result += ' ';
		}
	}
	return result;
}

std::string RomajiTyper::ToRomaji(const std::string& reading)
{
	RomajiTyper typer;
	if (!typer.Reset(reading))
	{
		return std::string();
	}
	return typer.GetRemaining();
}
