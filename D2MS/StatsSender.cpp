#include "stdafx.h"
#include "StatsSender.h"
#include "D2Structs.h"
#include "D2ptrs.h"
//#include "ItemParser.h"
#include "D2Helpers.h"

#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>

using namespace std;

static DWORD WINAPI StaticThreadStart2(void* Param);
HANDLE hPipe;

StatsSender::StatsSender(void)
{
	//
}

void StatsSender::Init(void)
{
	b = true;



	LPTSTR lpszPipename = "\\\\.\\Pipe\\D2MSPipe";
	
	hPipe = ::CreateNamedPipe(lpszPipename,
								PIPE_ACCESS_OUTBOUND,
								PIPE_TYPE_MESSAGE,// | PIPE_READMODE_MESSAGE,
								PIPE_UNLIMITED_INSTANCES,
								4096,
								4096,
								0,
								NULL);



	/*
	if (hPipe == INVALID_HANDLE_VALUE)
	return 0;*/

	ConnectNamedPipe(hPipe, NULL);
}


StatsSender::~StatsSender(void)
{
	CloseHandle(hPipe);
}

unordered_map<unsigned int, unsigned __int64> _pickedItems;
unordered_map<int, int> _timers;


//char *JsonData = "{\"data\":{\"AccountName\":\"\",\"CharName\":\"SilverDeth-IV\",\"GameDiff\":\"1\",\"CompressedData\":[8228138568842941382,8247906688399250381,8244242016143283142]}}\n";

void StatsSender::SendStats(unordered_map<unsigned int, unsigned __int64> pickedItems, unordered_map<int, int> timers){

	_pickedItems = pickedItems;
	_timers = timers;


	pickedItems.clear();
	timers.clear();

	HANDLE h = CreateThread(NULL, 0, StaticThreadStart2, (void*) this, 0, 0);
}

void RunThread2()
{
	//int gameDiff = D2CLIENT_GetDifficulty();
	int gameDiff = 1;
	BnetData* pData = (*p_D2LAUNCH_BnData);

	char* accountNameChar = pData->szAccountName;
	char* charName = pData->szPlayerName; //GetCharName();

	std::string accountNameCharString(accountNameChar);
	std::string charNameCharString(charName);


	string jsonData = "{\"data\":{\"AccountName\":\"" + accountNameCharString + "\",\"CharName\":\"" + charNameCharString + "\",";
	jsonData += "\"GameDiff\":\"" + std::to_string(gameDiff) + "\",\"CompressedData\":[";



	unordered_map<unsigned int, unsigned __int64>::const_iterator it;

	for (it = _pickedItems.begin(); it != _pickedItems.end(); it++) {
		jsonData += "{\"ItemKey\":\"" + std::to_string((*it).first) + "\", \"ItemValue\":\"" + std::to_string((*it).second) + "\"},";

	}

	if (_pickedItems.size() > 0) {
		jsonData.pop_back(); // remove last , 
	}


	jsonData += "], \"Timers\":[";


	unordered_map<int, int>::const_iterator it2;

	for (it2 = _timers.begin(); it2 != _timers.end(); it2++) {
		jsonData += "{\"LvlNo\":\"" + std::to_string((*it2).first) + "\", \"Time\":\"" + std::to_string((*it2).second) + "\"},";
	}

	if (_timers.size() > 0) {
		jsonData.pop_back(); // remove last , 
	}

	jsonData += "]}}\n";


	//_oldItems.clear();


	//SetCharName(""); // will be reset in core loop the fuck is this


	DWORD dwWrite, dwRead;

	bool flg = WriteFile(hPipe, jsonData.c_str(), jsonData.size(), &dwWrite, NULL);

}

static DWORD WINAPI StaticThreadStart2(void* Param)
{
	RunThread2();
	return 0;
}
