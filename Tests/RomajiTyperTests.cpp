// RomajiTyper の単体テスト。失敗した数を終了コードで返す。
#include <cstdio>
#include <string>
#include "../ConsoleTyping/RomajiTyper.h"

namespace
{
	int gFailures = 0;

	void Check(bool condition, const std::string& label)
	{
		if (!condition)
		{
			gFailures++;
			std::printf("FAIL: %s\n", label.c_str());
		}
	}

	// reading を input どおりに打ち、1 度も外さず最後で Done になるか
	bool TypesAll(const std::string& reading, const std::string& input)
	{
		RomajiTyper typer;
		if (!typer.Reset(reading))
		{
			return false;
		}
		for (size_t i = 0; i < input.size(); i++)
		{
			RomajiTyper::Result result = typer.Input(input[i]);
			if (result == RomajiTyper::Result::Miss)
			{
				return false;
			}
			if (result == RomajiTyper::Result::Done)
			{
				return i + 1 == input.size();
			}
		}
		return false;
	}

	// 途中までは通るが、まだ終わらないか
	bool TypesPartial(const std::string& reading, const std::string& input)
	{
		RomajiTyper typer;
		if (!typer.Reset(reading))
		{
			return false;
		}
		for (char c : input)
		{
			if (typer.Input(c) != RomajiTyper::Result::Hit)
			{
				return false;
			}
		}
		return !typer.IsDone();
	}

	// input の最後の 1 文字だけが Miss になるか
	bool MissesAtLast(const std::string& reading, const std::string& input)
	{
		RomajiTyper typer;
		if (!typer.Reset(reading))
		{
			return false;
		}
		for (size_t i = 0; i + 1 < input.size(); i++)
		{
			if (typer.Input(input[i]) == RomajiTyper::Result::Miss)
			{
				return false;
			}
		}
		return typer.Input(input.back()) == RomajiTyper::Result::Miss;
	}
}

int main()
{
	// 基本と複数の打ち方
	Check(TypesAll(u8"しんぶん", "shinbunn"), "shinbunn");
	Check(TypesAll(u8"しんぶん", "sinbunn"), "sinbunn");
	Check(TypesAll(u8"しんぶん", "sinnbunn"), "sinnbunn");
	Check(TypesAll(u8"ちち", "titi"), "titi");
	Check(TypesAll(u8"ちち", "chichi"), "chichi");
	Check(TypesAll(u8"つくえ", "tukue"), "tukue");
	Check(TypesAll(u8"ふじ", "huzi"), "huzi");

	// 長音は - キー
	Check(TypesAll(u8"こんぴゅーた", "konpyu-ta"), "konpyu-ta");
	Check(TypesAll(u8"こんぴゅーた", "konpixyu-ta"), "konpixyu-ta");

	// 「ん」: 次が子音なら n 1 文字、母音・n・y なら nn / xn が必要
	Check(TypesAll(u8"かんじ", "kanji"), "kanji");
	Check(TypesAll(u8"かんじ", "kannji"), "kannji (extra n is accepted)");
	Check(TypesAll(u8"かんじ", "kaxnji"), "kaxnji");
	Check(TypesAll(u8"ほん", "honn"), "honn at the end");
	Check(TypesPartial(u8"ほん", "hon"), "hon at the end is not enough");
	Check(TypesAll(u8"こんいん", "konninn"), "konninn");
	Check(MissesAtLast(u8"こんあ", "kona"), "kona: n before a must be doubled");
	Check(TypesAll(u8"こんにちは", "konnnichiha"), "konnnichiha");
	Check(TypesAll(u8"こんにちは", "konnichiha"), "konnichiha: n + ni");
	Check(MissesAtLast(u8"こんにちは", "koni"), "koni: a single n is not enough for n + ni");
	Check(TypesAll(u8"こんな", "konna"), "konna");
	Check(TypesAll(u8"こんな", "konnna"), "konnna");
	Check(TypesAll(u8"こんな", "koxnna"), "koxnna");
	Check(TypesAll(u8"じゅんばんに", "junbannni"), "junbannni");
	Check(TypesAll(u8"じゅんばんに", "junbanni"), "junbanni (the display route)");
	Check(TypesAll(u8"こんや", "konnya"), "konnya");

	// 「っ」
	Check(TypesAll(u8"きって", "kitte"), "kitte");
	Check(TypesAll(u8"きって", "kixtute"), "kixtute");
	Check(TypesAll(u8"きって", "kiltute"), "kiltute");
	Check(TypesAll(u8"まっちゃ", "matcha"), "matcha");
	Check(TypesAll(u8"まっちゃ", "maccha"), "maccha");
	Check(TypesAll(u8"いっぱい", "ippai"), "ippai");
	Check(TypesAll(u8"あっ", "axtu"), "axtu (standalone small tsu)");
	Check(MissesAtLast(u8"あっ", "at"), "standalone small tsu cannot be doubled");

	// 拗音・外来音
	Check(TypesAll(u8"しゃしん", "shashinn"), "shashinn");
	Check(TypesAll(u8"しゃしん", "syasinn"), "syasinn");
	Check(TypesAll(u8"じゅう", "juu"), "juu");
	Check(TypesAll(u8"ふぁいる", "fairu"), "fairu");
	Check(TypesAll(u8"てぃっしゅ", "thissyu"), "thissyu");

	// 外したキーは進まない
	{
		RomajiTyper typer;
		typer.Reset(u8"ねこ");
		Check(typer.Input('x') == RomajiTyper::Result::Miss, "x misses");
		Check(typer.GetTyped().empty(), "miss does not advance");
		Check(typer.Input('n') == RomajiTyper::Result::Hit, "n hits");
		Check(typer.Input('k') == RomajiTyper::Result::Miss, "k misses after n");
		Check(typer.GetTyped() == "n", "typed after miss");
	}

	// 表示: 打った経路に合わせて残りが変わる
	{
		RomajiTyper typer;
		typer.Reset(u8"しち");
		Check(typer.GetRemaining() == "shichi", "default remaining");
		typer.Input('s');
		Check(typer.GetTyped() == "s" && typer.GetRemaining() == "hichi", "remaining after s");
		typer.Input('i');
		Check(typer.GetTyped() == "si" && typer.GetRemaining() == "chi", "typed si follows chosen path");
	}

	// スペースは表示だけで、打たなくてよい
	{
		RomajiTyper typer;
		typer.Reset(u8"きー ぼーど");
		Check(typer.GetRemaining() == "ki- bo-do", "spaces are shown");
		Check(TypesAll(u8"きー ぼーど", "ki-bo-do"), "spaces are skipped");
		typer.Input('k');
		typer.Input('i');
		typer.Input('-');
		Check(typer.GetTyped() == "ki- ", "typed includes the space");
	}

	// 半角の英数字
	Check(TypesAll(u8"ぱそこん 2 だい", "pasokonn2dai"), "ascii digits");

	// 使えない文字
	{
		RomajiTyper typer;
		std::string error;
		Check(!typer.Reset(u8"漢字", &error) && !error.empty(), "kanji is rejected");
		Check(!typer.Reset(u8"カタカナ"), "katakana is rejected");
		Check(!typer.Reset(""), "empty is rejected");
		Check(RomajiTyper::ToRomaji(u8"こんぴゅーた") == "konpyu-ta", "ToRomaji");
	}

	if (gFailures == 0)
	{
		std::printf("OK: all RomajiTyper tests passed\n");
	}
	return gFailures;
}
