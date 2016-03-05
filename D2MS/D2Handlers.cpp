#pragma once
#include "stdafx.h"
#include "ItemParser.h"

#include "D2ptrs.h"
#include "D2Helpers.h"
#include "Core.h"

#include "FileLogger.cpp"



//bp d2client + 0004EA0B
DWORD __fastcall MSGamePacketReceived(BYTE* pPacket, DWORD dwSize)
{
	CheckTimer(); // update timers according to room, could call this for only some packets.


	switch(pPacket[0])
	{
		
		case 0x9C:
		case 0x9D: 
					
			ParseItem(pPacket);
			
			//QueueItem(pPacket);
			//testItems->push(pPacket);

	}

	return TRUE;
}




void GameLeave(void)
{
	CoreGameLeave();
	ItemParserGameLeave();
}

