/*
Figurer laddresse
MHS
mhs -> disambler
export -> d2client
right click goto + le offset

le add au goto est dans le name de sa function


IMPORTANT PROJECT SETTING -> GENERAL :
	For normal multi-byte select:
	Use Multi-Byte Character Set

TO DEBUG THE NATIVE DLL
Start .net project.
debug-> attach to process-> game.exe
inject native dll in game
should breakpoints.

*/

//right click project, vs2012 ou vs2011


#pragma once
#include "stdafx.h"

#include <windows.h>
#include <windowsx.h>
#include <thread>

#include "ConfigFile.h"
#include "D2MS.h"

#define _DEFINE_PTRS


#include "D2ptrs.h"
#include "D2Intercepts.h"
#include "D2Handlers.h"
#include "Core.h"


#include "D2Helpers.h"

using namespace std;


D2MS::D2MS(void){

}


D2MS::~D2MS(void){
	//delete mylog;
}



Patch* patches[] = {
	new Patch(Call, D2CLIENT, 0x4515D, (DWORD)GameLeave, 5),
	new Patch(Call, D2CLIENT, 0xACE61, (int)PacketReceivedWrapperCall, 5) //WORKING FOR BH SUPPORT
	//new Patch(Call, D2CLIENT, 0xACE61, (int)MSGamePacketReceived_Intercept, 5) //WORKING FOR STAND ALONE APP
};

//static DWORD WINAPI StaticThreadStart(void* Param);
void InstallPatches();


void D2MS::Init(){
	
	ConfigFile cf("D2MS.cfg");

	bool verboseConfig = (bool)cf.Value("General", "verbose");

	SetVerbose(true);

	InstallPatches();

	if(ClientState() == ClientStateInGame) //ClientStateInGame)
		D2CLIENT_PrintGameString(L"D2MS Successfully Loaded", Color::Orange);
	
	Core c;
	c.Run();
}

void InstallPatches(){
	for (int n = 0; n < (sizeof(patches) / sizeof(Patch*)); n++) {
		patches[n]->Install();
	}
}

