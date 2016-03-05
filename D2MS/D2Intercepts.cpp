#pragma once
#include "stdafx.h"
#include "D2Intercepts.h"
#include "D2Handlers.h"


void __declspec(naked) GameLeave_Intercept(void)
{
	__asm
	{
		call GameLeave
		jmp D2CLIENT_GameLeave_I
	}
}


//WORKING FOR STAND ALONE APP AKA NO BH SUPPORT

//
//void __declspec(naked) MSGamePacketReceived_Intercept()
//{
//	__asm
//	{
//		pop ebp;
//		pushad;
//
//		call MSGamePacketReceived;
//		test eax, eax;
//
//		popad;
//		jnz OldCode;
//
//		mov edx, 0;
//
//	OldCode:
//		call D2NET_ReceivePacketTest_Ix;
//
//		push ebp;
//		ret;
//	}
//}




//*********************************************
//working for MH Support
//*********************************************

//execute my code then call bh after this
void __declspec(naked) MSGamePacketReceived_Intercept()
{
	__asm
	{
		pop ebp;
		pushad;

		call MSGamePacketReceived;
		test eax, eax;

		popad;
		jnz OldCode;

		mov edx, 0;

	OldCode:

		push ebp;
		ret;
	}
}

void __declspec(naked) PacketReceivedWrapperCall(void)
{
	__asm
	{
		call MSGamePacketReceived_Intercept;
		jmp BH_PacketReceived;
	}
}



//goto BH.DLL + 00012232 BH_PacketReceived
