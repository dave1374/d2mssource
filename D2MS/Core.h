#pragma once
#include <string>
#include <unordered_map>

class Core
{
public:	
	void Run();
	//void RunThread();

	Core(void);
	~Core(void);
};

void CoreGameLeave();
int GetTimerRoomLvlNo();
void CheckTimer();

//void QueueItem(BYTE* pPacket);