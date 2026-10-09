#include "ProblemSet.h"
#include <filesystem>
#include <fstream>
#include "RomajiTyper.h"

namespace
{
	// タイピング画面のローマ字表示に収まる目安の文字数
	const size_t MaxRomajiLength = 60;

	std::string Trim(const std::string& text)
	{
		size_t begin = 0;
		size_t end = text.size();
		while (begin < end && (text[begin] == ' ' || text[begin] == '\t' || text[begin] == '\r' || text[begin] == '\n'))
		{
			begin++;
		}
		while (end > begin && (text[end - 1] == ' ' || text[end - 1] == '\t' || text[end - 1] == '\r' || text[end - 1] == '\n'))
		{
			end--;
		}
		return text.substr(begin, end - begin);
	}
}

void ProblemSet::Load(const std::string& path, std::vector<std::string>* warnings)
{
	mProblems.clear();

	auto warn = [warnings](const std::string& message)
	{
		if (warnings)
		{
			warnings->push_back(message);
		}
	};

	std::ifstream file(std::filesystem::u8path(path), std::ios::binary);
	if (!file)
	{
		warn("problems file not found: " + path);
	}
	else
	{
		std::string line;
		int lineNumber = 0;
		while (std::getline(file, line))
		{
			lineNumber++;
			// UTF-8 の BOM は先頭行だけ取り除く
			if (lineNumber == 1 && line.compare(0, 3, "\xEF\xBB\xBF") == 0)
			{
				line.erase(0, 3);
			}

			const std::string trimmed = Trim(line);
			if (trimmed.empty() || trimmed[0] == '#')
			{
				continue;
			}

			const size_t tab = trimmed.find('\t');
			if (tab == std::string::npos)
			{
				warn("problems line " + std::to_string(lineNumber) + ": TAB separator is missing");
				continue;
			}

			Problem problem;
			problem.text = Trim(trimmed.substr(0, tab));
			problem.reading = Trim(trimmed.substr(tab + 1));

			std::string error;
			RomajiTyper typer;
			if (problem.text.empty() || !typer.Reset(problem.reading, &error))
			{
				warn("problems line " + std::to_string(lineNumber) + ": invalid reading (" + error + ")");
				continue;
			}

			// 画面の幅に収まらないローマ字は途中で切れてしまうので、警告だけ出す
			if (RomajiTyper::ToRomaji(problem.reading).size() > MaxRomajiLength)
			{
				warn("problems line " + std::to_string(lineNumber) + ": romaji is longer than "
					+ std::to_string(MaxRomajiLength) + " characters and may not fit on the screen");
			}
			mProblems.push_back(problem);
		}
	}

	if (mProblems.empty())
	{
		warn("no valid problems; using the built-in problem");
		mProblems.push_back({ u8"タイピングを始めよう", u8"たいぴんぐを はじめよう" });
	}
}

const Problem& ProblemSet::At(size_t index) const
{
	return mProblems[index % mProblems.size()];
}
