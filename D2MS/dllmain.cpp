//if you inject a DLL in another process, you have to attach the debugger to THAT 
//process before the injection. Use "Debug"->"Attach to process..." Breakpoint will be hit. 

//debug project setting
//debugging attach yes
//optimisation non
//c++ general debug information : none


//release proj settings
// o2(optimized for speed)
//c++ general debug information put ZI for debug
//c++ general all options -> basic run tikme check a both for debug 

#include "stdafx.h"
#include "D2MS.h"
#include <fstream>
#include "FileLogger.cpp"

#include <sstream>
#include <iostream>
#include <fstream>
using namespace std;

void MainThread();

static DWORD WINAPI StaticThreadStart(void* Param)
{
	MainThread();
	return 0xfff;
}

BOOL APIENTRY DllMain( HMODULE hModule,  DWORD  ul_reason_for_call, LPVOID lpReserved )
{	

	ofstream myfile;
	myfile.open("debugd2ms.txt");
	myfile << "1\n";

	if(ul_reason_for_call == DLL_PROCESS_ATTACH){

		myfile << "2\n";
		HANDLE h = CreateThread(NULL, 0, StaticThreadStart, NULL, 0, 0);

		myfile << "3\n";
	}

	/*switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:		
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
	}*/


	myfile.close();

	return TRUE;
}

void MainThread(){
	ofstream myfile;
	myfile.open("debugd2ms.txt");
	myfile << "4\n";


	D2MS* cc = new D2MS();
	(*cc).Init();

	myfile << "5\n";


	myfile.close();
}