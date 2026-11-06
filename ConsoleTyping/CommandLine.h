#pragma once

#include <string>

// メインメニューで打ったコマンド行の解釈。SDL に依存しないので単体テストできる
namespace CommandLine
{
	// 前後の空白を除いた最初の単語。単語がなければ空文字
	std::string FirstWord(const std::string& line);
}
