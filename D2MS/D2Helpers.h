#ifndef D2HELPERS_H
#define D2HELPERS_H

#include "D2Structs.h"
#include "D2Ptrs.h"

#include <string>
#include <locale>
#include <codecvt>

enum ClientGameState
{
	ClientStateNull, ClientStateMenu, ClientStateInGame, ClientStateBusy
};

ClientGameState ClientState(void);

UnitAny* D2CLIENT_FindUnit(DWORD dwId, DWORD dwType);

const char* GetUnitName(UnitAny* pUnit, char* szBuf, size_t bufSize);
void GetItemCode(UnitAny* pUnit, char* szBuf);

char* UnicodeToAnsi(const std::wstring wstring);
wchar_t* AnsiToUnicode(const char* str);
std::string UnicodeToString(const wchar_t* str);


char* GetCharName();
void  SetCharName(char* newCharName);

bool GetVerbose();
void SetVerbose(bool b);


static inline void D2CLIENT_PrintGameInt(int i, Color c) { 
	wchar_t* tmp = AnsiToUnicode( std::to_string(i).c_str() );
	D2CLIENT_PrintGameString(tmp, Color::Orange);
	delete tmp; 
}

/*
static inline DWORD __stdcall MSGetCurrentLevelNo() { 
	Room1* pRoom = D2COMMON_GetRoomFromUnit(D2CLIENT_GetPlayerUnit());
	
	if(!pRoom || !pRoom->pRoom2 || !D2CLIENT_GetPlayerUnit()->pPath->pRoom1 || !pRoom->pRoom2->pLevel)
	{
		return NULL;
	}

	return D2CLIENT_GetPlayerUnit()->pPath->pRoom1->pRoom2->pLevel->dwLevelNo; 
}*/



#endif