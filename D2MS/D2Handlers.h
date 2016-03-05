#pragma once
#include <windows.h>

//DWORD ItemActionHandler(BYTE* pPacket, DWORD dwSize);
DWORD __fastcall MSGamePacketReceived(BYTE* pPacket, DWORD dwSize);
void GameLeave(void);