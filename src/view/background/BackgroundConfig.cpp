#include "BackgroundConfig.h"

#include <boost/algorithm/string.hpp>

#include <vector>
using std::vector;
using std::pair;

BackgroundConfig::BackgroundConfig(string config)
{
	XOJ_INIT_TYPE(BackgroundConfig);

	vector<string> parts;
	boost::split(parts, config, boost::is_any_of(","));

	for (string s : parts)
	{
		size_t dotPos = s.find_last_of("=");
		if (dotPos != string::npos)
		{
			string key = s.substr(0, dotPos);
			string value = s.substr(dotPos + 1);
			data[key] = value;
		}
	}
}

BackgroundConfig::~BackgroundConfig()
{
	XOJ_CHECK_TYPE(BackgroundConfig);

	XOJ_RELEASE_TYPE(BackgroundConfig);
}

bool BackgroundConfig::loadValue(string key, string& value)
{
	XOJ_CHECK_TYPE(BackgroundConfig);

	auto it = data.find(key);
	if (it != this->data.end())
	{
		value = it->second;
		return true;
	}

	return false;
}

bool BackgroundConfig::loadValue(string key, int& value)
{
	XOJ_CHECK_TYPE(BackgroundConfig);

	string str;
	if (loadValue(key, str))
	{
		value = std::stoul(str, nullptr, 10);
		return true;
	}

	return false;
}

bool BackgroundConfig::loadValue(string key, double& value)
{
	XOJ_CHECK_TYPE(BackgroundConfig);

	string str;
	if (loadValue(key, str))
	{
		value = std::stoul(str, nullptr, 10);
		return true;
	}

	return false;
}

bool BackgroundConfig::loadValueHex(string key, int& value)
{
	XOJ_CHECK_TYPE(BackgroundConfig);

	string str;
	if (loadValue(key, str))
	{
		value = std::stoul(str, nullptr, 16);
		return true;
	}

	return false;
}

