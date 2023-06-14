#include "cryption.h"


using namespace mokdong;

void simple_cryption::encrypt(char* start, char* end)
{
	int key = simple_cryption::key >> 4;

	for (char* ptr = start; ptr != end; ptr++)
	{
		*ptr = *ptr ^ key;
		key += *ptr;
	}

}

void simple_cryption::decrypt(char* start, char* end)
{
	int key = simple_cryption::key >> 4;

	char pre = 0;
	for (char* ptr = start; ptr != end; ptr++)
	{
		pre = *ptr;
		*ptr = *ptr ^ key;
		key += pre;
	}
}
