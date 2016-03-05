#pragma once
#include "stdafx.h"
#include "Core.h"

#include "D2Helpers.h"
#include "StatsSender.h"
#include "ItemParser.h"
#include <queue>   

using namespace std;

unsigned int roomLvl = 0;
clock_t timeInRoom;

Core::Core(void)
{

}

std::unordered_map<int, int> LevelsTimers;


void Core::Run(){

	timeInRoom = clock();

}

void CheckTimer() {

	UnitAny* pPlayer = D2CLIENT_GetPlayerUnit();
	if (!pPlayer)
	{
		return;
	}

	Room1* pRoom = D2COMMON_GetRoomFromUnit(D2CLIENT_GetPlayerUnit());
	if (!pRoom || !pRoom->pRoom2 || !pRoom->pRoom2->pLevel)
	{
		return;
	}

	unsigned __int8 tmpLvlNo = pRoom->pRoom2->pLevel->dwLevelNo;

	if (roomLvl == 0)
	{

		roomLvl = tmpLvlNo;
		timeInRoom = clock(); // start new timer

		return;
	}

	if (tmpLvlNo != roomLvl) { // changed room since last item packet

		
		timeInRoom = ((float) clock() - timeInRoom) / CLOCKS_PER_SEC;

		//save time in list

		unordered_map<int, int>::iterator it = LevelsTimers.find(roomLvl);

		if (it != LevelsTimers.end())
			it->second = (it->second + timeInRoom); //add time spent in old room
		else
			LevelsTimers.insert(std::make_pair(roomLvl, timeInRoom)); //first time being in last room

		roomLvl = tmpLvlNo;
		timeInRoom = clock(); // start new timer

	}
}


void CoreGameLeave()
{

	/*
	log time in last room
	*/
	

	unordered_map<int, int>::iterator it = LevelsTimers.find(roomLvl);

	timeInRoom = ((float)clock() - timeInRoom) / CLOCKS_PER_SEC;

	if (it != LevelsTimers.end())
		it->second = (it->second + timeInRoom); //add time spent in old room
	else
		LevelsTimers.insert(std::make_pair(roomLvl, timeInRoom)); //first time being in last room


	roomLvl = 0;
	timeInRoom	= 0;


	SetLevelsTimers(LevelsTimers);
	
	LevelsTimers.clear();
}

int GetTimerRoomLvlNo(){
	return roomLvl;
}

Core::~Core(void)
{
}