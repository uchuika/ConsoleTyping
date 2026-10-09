#pragma once

#include <map>
#include <string>

// シーン間で受け渡す値
class Parameter
{
public:
	static const int Error = -1;

	Parameter() = default;
	virtual ~Parameter() = default;

	void set(const std::string& key, int val);
	int get(const std::string& key) const;

private:
	std::map<std::string, int> mMap;
};
