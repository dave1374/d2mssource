#pragma once
#include <string>
#include <unordered_set>

using namespace std;

class ParserConfig
{
public:
	std::unordered_set<string> ignoredItems;

	ParserConfig(string s);
	~ParserConfig();
};
