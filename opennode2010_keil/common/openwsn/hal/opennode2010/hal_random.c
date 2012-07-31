#include "../hal_random.h"
#include "../hal_configall.h"
#include <string.h>
#include "../hal_foundation.h"
#include "../hal_mcu.h"
#include "hal_cc2520base.h"


uint8 hal_random_uint8()
{
	return(CC2520_RANDOM8());
}

uint16 hal_random_uint16()
{
	uint8 ran1 = CC2520_RANDOM8();
	uint8 ran2 = CC2520_RANDOM8();
	uint16 random = (ran1<<8) | ran2;
	return random; 
}

uint32 hal_random_uint32()
{
	uint16 ran1 = hal_random_uint16();
	uint16 ran2 = hal_random_uint16();
	uint32 random = (ran1<<16) | ran2;
	return random;
}

uint8 hal_rand_uint8(uint8 n)
{
	return(CC2520_RANDOM8()% n);
}
