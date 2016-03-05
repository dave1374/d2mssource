#include "stdafx.h"
#include "ParserConfig.h"
#include <sstream>

ParserConfig::ParserConfig(string s)
{

	std::stringstream ss(s);

	string token;


	while (std::getline(ss, token, ',')) {
		ignoredItems.insert(token);	
	}

}


ParserConfig::~ParserConfig()
{

}
