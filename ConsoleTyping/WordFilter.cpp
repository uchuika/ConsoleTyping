#include "WordFilter.h"
#include <filesystem>
#include <fstream>

namespace
{
	std::string ToLowerAscii(std::string text)
	{
		for (char& c : text)
		{
			if (c >= 'A' && c <= 'Z')
			{
				c = static_cast<char>(c - 'A' + 'a');
			}
		}
		return text;
	}

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

bool WordFilter::Load(const std::string& path)
{
	mWords.clear();

	std::ifstream file(std::filesystem::u8path(path), std::ios::binary);
	if (!file)
	{
		return false;
	}

	std::string line;
	bool first = true;
	while (std::getline(file, line))
	{
		if (first && line.compare(0, 3, "\xEF\xBB\xBF") == 0)
		{
			line.erase(0, 3);
		}
		first = false;

		const std::string word = Trim(line);
		if (word.empty() || word[0] == '#')
		{
			continue;
		}
		mWords.push_back(ToLowerAscii(word));
	}
	return true;
}

bool WordFilter::ContainsNgWord(const std::string& text) const
{
	const std::string lowered = ToLowerAscii(text);
	for (const std::string& word : mWords)
	{
		if (lowered.find(word) != std::string::npos)
		{
			return true;
		}
	}
	return false;
}
