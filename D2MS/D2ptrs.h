
#pragma once
// Macros adapted from lord2800's macros.
#include "Patch.h"
#include "D2Structs.h"


enum Color
{
	White, Red, Green, Blue, Gold, Grey, Black, Brown, Orange, Yellow, Purple
};

#ifdef _DEFINE_PTRS
#define FUNCPTR(dll, name, callingret, args, address) \
	__declspec(naked) callingret dll##_##name##args \
	{ \
		static DWORD f##dll##_##name = NULL; \
		if(f##dll##_##name == NULL) \
		{ \
		__asm { pushad } \
		f##dll##_##name = Patch::GetDllOffset(dll, address); \
		__asm { popad } \
		} \
		__asm jmp [f##dll##_##name] \
	}

#define ASMPTR(dll, name, address) \
	DWORD* Asm_##dll##_##name##(VOID) \
	{ \
		static DWORD f##Asm_##dll##_##name = NULL; \
		if(f##Asm_##dll##_##name## == NULL) \
		{ \
		f##Asm_##dll##_##name## = Patch::GetDllOffset(dll, address); \
		} \
		return &##f##Asm_##dll##_##name; \
	} 

#define VARPTR(dll, name, type, address) \
	type** Var_##dll##_##name##(VOID) \
	{ \
		static DWORD f##Var_##dll##_##name = NULL; \
		if(f##Var_##dll##_##name## == NULL) \
		{ \
		f##Var_##dll##_##name## = Patch::GetDllOffset(dll, address); \
		} \
		return (type**)&##f##Var_##dll##_##name; \
	} 

#else
#define FUNCPTR(dll, name, callingret, args, address) extern callingret dll##_##name##args;
#define ASMPTR(dll, name, address) extern DWORD* Asm_##dll##_##name##(VOID); static DWORD dll##_##name = *Asm_##dll##_##name##();
#define VARPTR(dll, name, type, address) extern type** Var_##dll##_##name##(VOID); static type* p##_##dll##_##name = (type*)*Var_##dll##_##name##();
#endif



FUNCPTR(D2CLIENT, PrintGameString, void __stdcall, (wchar_t *wMessage, int nColor), 0x7D850)
FUNCPTR(D2CLIENT, GetPlayerUnit, UnitAny*  __stdcall,(),0xA4D60) // Updated 1.13c
FUNCPTR(D2NET, ReceivePacketTest_Ix, void __fastcall, (BYTE *buffer, DWORD buflen, DWORD* packetSize), -10033)
FUNCPTR(D2COMMON, GetRoomFromUnit,  Room1* __stdcall, (UnitAny * ptUnit), -10331)
FUNCPTR(BH, PacketReceived, void  __stdcall, (), 0x00012A10) //Version 0.1.5s
//FUNCPTR(BH, PacketReceived, void  __stdcall, (), 0x00012230) // Version 0.1.5 ??? not sure old mh version 
//FUNCPTR(BH, PacketReceived, void  __stdcall, (), 0x00050C20) //Version 0.1.7e


ASMPTR(D2CLIENT, GameLeave_I, 0x5D110)
VARPTR(D2LAUNCH, BnData, BnetData *, 0x25ABC)

//
//steps for bh offset
//convertir - 10033 en int ( MHS le convert quand on le met en offset GOTO dll en debug mode )
//en hexa
//12609360
//C06750
//d2net.dll + 6750
//add breakpoint
//a gauche dans stack
//return to bh.dll 0F032246 ............... 0FC82A26
//

//bh.dll + 00012230
//bh.dll + 00012A10



//FUNCPTR(D2CLIENT, ExitGame, void __fastcall, (void), 0x42850)

//FUNCPTR(D2CLIENT, GetItemName, BOOL __stdcall, (UnitAny* pItem, wchar_t* wBuffer, DWORD dwSize), 0x914F0)
//FUNCPTR(D2CLIENT, FindClientSideUnit, UnitAny* __fastcall, (DWORD dwId, DWORD dwType), 0xA5B20)
//FUNCPTR(D2CLIENT, FindServerSideUnit, UnitAny* __fastcall, (DWORD dwId, DWORD dwType), 0xA5B40)
//FUNCPTR(D2CLIENT, GetDifficulty, BYTE __stdcall, (void), 0x41930)
//FUNCPTR(D2COMMON, GetItemMagicalMods, char* __stdcall, (WORD wPrefixNum), -10248)
//FUNCPTR(D2COMMON, GetLevelText, LevelTxt * __stdcall, (DWORD levelno), -10014) // or +0x6CCC0