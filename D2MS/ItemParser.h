#pragma once

#include <windows.h>
#include "D2Structs.h"
#include <unordered_map>

using namespace std;

bool ParseItem(const unsigned char *data) ;
//void ProcessItem(ItemInfo* item);
void ItemParserGameLeave();
void SetLevelsTimers(std::unordered_map<int, int> timers);
