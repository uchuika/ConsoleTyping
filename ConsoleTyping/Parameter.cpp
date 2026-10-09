#include "Parameter.h"

void Parameter::set(const std::string& key, int val)
{
	mMap[key] = val;
}

int Parameter::get(const std::string& key) const
{
	auto iter = mMap.find(key);
	if (iter == mMap.end())
	{
		return Error;
	}
	return iter->second;
}
