#include "global.h"


using namespace mokdong;
using namespace mokdong::network;

void global::purge()
{
	room_manager().purge();
}