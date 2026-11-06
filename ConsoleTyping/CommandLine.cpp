#include "CommandLine.h"

std::string CommandLine::FirstWord(const std::string& line)
{
	const size_t begin = line.find_first_not_of(' ');
	if (begin == std::string::npos)
	{
		return std::string();
	}
	const size_t end = line.find(' ', begin);
	return line.substr(begin, end == std::string::npos ? std::string::npos : end - begin);
}
