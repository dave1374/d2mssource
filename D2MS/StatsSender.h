#pragma once
#include "D2Structs.h"

#include <unordered_map>
//
//
//#import "D2MSDataLayer.tlb" raw_interfaces_only
//
//using namespace D2MSDataLayer;
//using namespace std;


class StatsSender
{
public:
	bool b = false;
	//IDataSenderPtr _D2MSDataSender;
	
	StatsSender(void);
	~StatsSender(void);
		
	void StatsSender::SendStats(std::unordered_map<unsigned int, unsigned __int64> pickedItems, std::unordered_map<int, int> timers);
	void Init(void);
};