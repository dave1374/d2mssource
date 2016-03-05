#include "stdafx.h"
#include "D2Helpers.h"

char* charName = "";
bool verbose = false;



wchar_t* AnsiToUnicode(const char* str)
{
	wchar_t* buf = NULL;
	int len = MultiByteToWideChar(CP_ACP, 0, (LPCCH)str, -1, buf, 0);
	buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, (LPCCH)str, -1, buf, len);
	return buf;
}

char* UnicodeToAnsi(const wchar_t* str)
{
	char* buf = NULL;
	int len = WideCharToMultiByte(CP_ACP, 0, str, -1, buf, 0, "?", NULL);
	buf = new char[len];
	WideCharToMultiByte(CP_ACP, 0, str, -1, buf, len, "?", NULL);
	return buf;
}

std::string UnicodeToString(const wchar_t* str)
{
	return std::string (UnicodeToAnsi(str));
}

bool GetVerbose(){
	return verbose;
}

void SetVerbose(bool b){
	verbose = b;
}

char* GetCharName(){
	return charName;
}

void SetCharName(char* newCharName){
	charName = newCharName;
}

ClientGameState ClientState(void)
{
	ClientGameState state = ClientStateNull;
	UnitAny* player = D2CLIENT_GetPlayerUnit();

	if(player != NULL)
	{
		if(player != NULL && player->pUpdateUnit != NULL)
		{
			state = ClientStateBusy;
			return state;
		}
		if (player->pInventory != NULL &&
			player->pPath != NULL &&
			player->pPath->pRoom1 != NULL &&
			player->pPath->pRoom1->pRoom2 != NULL &&
			player->pPath->pRoom1->pRoom2->pLevel != NULL &&
			player->pPath->pRoom1->pRoom2->pLevel->dwLevelNo != NULL)
			state = ClientStateInGame;
		else
			state = ClientStateBusy;
	}
	else if(player == NULL)
		state = ClientStateMenu;

	return state;
}

//UnitAny* D2CLIENT_FindUnit(DWORD dwId, DWORD dwType)
//{
//	if(dwId == -1) return NULL;
//	UnitAny* pUnit = D2CLIENT_FindServerSideUnit(dwId, dwType);
//	return pUnit ? pUnit : D2CLIENT_FindClientSideUnit(dwId, dwType);
//}