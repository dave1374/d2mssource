#include "stdafx.h"


#include "ParserConfig.h"
#include "ItemParser.h"
#include "D2ptrs.h"
#include "D2Helpers.h"
#include "Constant.h"
#include "BitReader.h"
#include <unordered_set>
#include <unordered_map>


#include "StatsSender.h"
#include "ConfigFile.h"

unordered_map<unsigned int, unsigned __int64> pickedItems;
pair<unordered_map<unsigned int, unsigned __int64>::iterator, bool> ret;


std::unordered_map<int, int> _timers2;

unordered_set<unsigned __int32> avalaibleItems;
unordered_set<unsigned __int32>::const_iterator itAvalaible;


StatsSender* ss = new StatsSender();
BitReader reader;



ConfigFile cf("D2MS.cfg");

string ignoreList = cf.Value("General", "ignoreList");

ParserConfig config = ParserConfig(ignoreList);
unordered_set<string>::const_iterator itConfig;


int lastPickedUpItem = -1;

bool ParseItem(const unsigned char *data) {

	//seems to get game item packet before room change happened when taking a wp
	Room1* pRoom = D2COMMON_GetRoomFromUnit(D2CLIENT_GetPlayerUnit());
	if ( !pRoom || !pRoom->pRoom2 || !pRoom->pRoom2->pLevel )
	{
		return false;
	}
	
	unsigned __int8 compactLvlNo = pRoom->pRoom2->pLevel->dwLevelNo;
	

	reader.init(data, 0);


	unsigned long packet = reader.read(8);
	unsigned int action = reader.read(8);

	

	if (action != ITEM_ACTION_TO_STORAGE && 
		action != ITEM_ACTION_NEW_GROUND &&
		action != ITEM_ACTION_OLD_GROUND &&
		action != ITEM_ACTION_DROP		 &&
		action != ITEM_ACTION_TO_STORE   &&
		action != ITEM_ACTION_FROM_STORAGE   &&
		action != ITEM_ACTION_SWAP_IN_CONTAINER)
		return false;
	
	//if in town and action is not to drop an item, 

	if ( (compactLvlNo	== MAP_A1_ROGUE_ENCAMPMENT	||
		compactLvlNo	== MAP_A2_LUT_GHOLEIN		||
		compactLvlNo	== MAP_A3_KURAST_DOCKS		||
		compactLvlNo	== MAP_A4_THE_PANDEMONIUM_FORTRESS ||
		compactLvlNo	== MAP_A5_HARROGATH)	&& 
		action			!= ITEM_ACTION_DROP		&& 
		action			!= ITEM_ACTION_TO_STORE &&
		action			!= ITEM_ACTION_FROM_STORAGE )
		return false;


	ItemInfo item;
	item.action = action;


	reader.offset += 8;//unsigned long messageSize = reader.read(8);

	//item.category = reader.read(8); // item type
	reader.offset += 8;

	item.id		  = reader.read(32);

	if (action == ITEM_ACTION_FROM_STORAGE)
	{
		lastPickedUpItem = item.id;
		return false;
	}
	
	if (action == ITEM_ACTION_NEW_GROUND ||
		action == ITEM_ACTION_OLD_GROUND)
	{
		avalaibleItems.insert(item.id);
		return false;
	}

	if (action == ITEM_ACTION_DROP || action == ITEM_ACTION_TO_STORE)
	{
		//add if is in pickeditems ?

		if (action == ITEM_ACTION_TO_STORE) {
			item.id = lastPickedUpItem;
		}

		unordered_map<unsigned int, unsigned __int64>::const_iterator got = pickedItems.find(item.id);

		if (!(got == pickedItems.end()))
		{
			pickedItems.erase(item.id);

			if(action == ITEM_ACTION_DROP) // do not re enable pick if action is sell.
				avalaibleItems.insert(item.id); // hmm id collision ?


			if (GetVerbose())
				D2CLIENT_PrintGameString(AnsiToUnicode("removed item"), Color::Red);
		}

		return false;
	}

	/*
	if (action == ITEM_ACTION_SWAP_IN_CONTAINER)
	{
		itAvalaible = avalaibleItems.find(item.id);

		//if it is ground to cursor to swap inventory
		if (itAvalaible != avalaibleItems.end())
		{


		}

		//else it is inventory to cursor, no fucks given.

		return false;
	}*/


	//here, action must be to inventory
	//check if its an item that was on the ground

	itAvalaible = avalaibleItems.find(item.id);

	if (itAvalaible == avalaibleItems.end())
	{		
		return false; // item was most likely moved from inventory to inventory
	}

	avalaibleItems.erase(item.id);
	
 	if (packet == 0x9d) {
		reader.offset +=32;
		reader.offset +=8;
	}

	/*
	reader.readBool(); //item->equipped = reader.readBool();
	reader.readBool();
	reader.readBool();
	reader.readBool(); //item->inSocket = reader.readBool();
	*/ 

	reader.offset += 4;

	item.identified = reader.readBool();
	
	/*
	reader.readBool();
	reader.readBool();//item->switchedIn = reader.readBool();
	reader.readBool();//item->switchedOut = reader.readBool();

	reader.readBool();//item->broken = reader.readBool();
	reader.readBool();
	reader.readBool();//item->potion = reader.readBool();
	reader.readBool();//item->hasSockets = reader.readBool();
	reader.readBool();
	reader.readBool();//item->inStore = reader.readBool();
	reader.readBool();//item->notInSocket = reader.readBool();
	reader.readBool();

	reader.readBool();//item->ear = reader.readBool();
	reader.readBool();//item->startItem = reader.readBool();
	reader.readBool();
	reader.readBool();
	reader.readBool();
	reader.readBool();//item->simpleItem = reader.readBool();
	reader.readBool();//item->ethereal = reader.readBool();
	reader.readBool();

	reader.readBool();//item->personalized = reader.readBool();
	reader.readBool();//item->gambling = reader.readBool();
	reader.readBool();//item->runeword = reader.readBool();
	reader.read(5);
	*/

	reader.offset += 27;

	//item.version = static_cast<unsigned int>(reader.read(8));
	reader.offset += 8;

	reader.offset += 2;
	unsigned long destination = reader.read(3);

	item.ground = (destination == 0x03);

	if (item.ground) {
		item.x = reader.read(16);
		item.y = reader.read(16);
	} else {
		item.directory = reader.read(4);
		item.x = reader.read(4);
		item.y = reader.read(3);
		item.container = static_cast<unsigned int>(reader.read(4));
	}

	item.unspecifiedDirectory = false;


	for (std::size_t i = 0; i < 4; i++) {
		item.code[i] = static_cast<char>(reader.read(8));
	}
	item.code[3] = 0;
	

	item.isGold = (item.code[0] == 'g' && item.code[1] == 'l' && item.code[2] == 'd');
	

	itConfig = config.ignoredItems.find(item.code);

	if (itConfig != config.ignoredItems.end())
	{
		return false; // ignore this item
	}


	/*if ( item.isGold || 
		(item.code[0] == 'm' && item.code[1] == 'p') || 
		(item.code[0] == 'h' && item.code[1] == 'p')		
		)
		return true;*/


	
	reader.offset += 10; // used sockets and level
	item.quality = static_cast<unsigned char>(reader.read(4));


	unsigned __int64 itemValue = 0;
		
	itemValue	  = (((unsigned __int64) item.code[0]) << 56) |
					(((unsigned __int64) item.code[1]) << 48) |
					(((unsigned __int64) item.code[2]) << 40) |
					(((unsigned __int64) compactLvlNo) << 32) |
					(((unsigned __int64) item.quality) << 28);



	char itemCode [4];
	itemCode[0] = item.code[0];
	itemCode[1] = item.code[1];
	itemCode[2] = item.code[2];
	itemCode[3] = item.code[3];

	

	ret = pickedItems.insert(pair<unsigned int, unsigned __int64>(item.id, itemValue));
	

	if (GetVerbose() && ret.second){// if second is true then item was inserted

		
			Color c;

			switch (item.quality)
			{

			case ITEM_QUALITY_MAGIC:
				c = Color::Blue;
				break;
			case ITEM_QUALITY_RARE:
				c = Color::Yellow;
				break;
			case ITEM_QUALITY_SET:
				c = Color::Green;
				break;
			case ITEM_QUALITY_UNIQUE:
				c = Color::Gold;
				break;

			default:
				c = Color::White;
				break;
			}

			D2CLIENT_PrintGameString(AnsiToUnicode(itemCode), c);

	}

	return true;



	/*if (ItemAttributeMap.find(item->code) == ItemAttributeMap.end()) {
		HandleUnknownItemCode(item->code, "from packet");
		*success = false;
		return;
	}*/

	//item->attrs = ItemAttributeMap[item->code];
	/*item->name = item->attrs->name;
	item->width = item->attrs->width;
	item->height = item->attrs->height;
	*/
	
	/*
	if (item->isGold) {
		bool big_pile = reader.readBool();
		if (big_pile) {
			item->amount = reader.read(32);
		} else {
			item->amount = reader.read(12);
		}
		return;
	}*/

	//item->usedSockets = (BYTE)reader.read(3);

	/*if (item->simpleItem || item->gambling) {
		return;
	}*/

	//item->level = (BYTE)reader.read(7);


	return true;
}

void ItemParserGameLeave()
{
	//if (pickedItems.size() > 0){
	if (ss->b == false)
		ss->Init();
		
	ss->SendStats(pickedItems, _timers2);

	pickedItems.clear();
	//}
}

void SetLevelsTimers(std::unordered_map<int, int> timers){
	_timers2 = timers;
}