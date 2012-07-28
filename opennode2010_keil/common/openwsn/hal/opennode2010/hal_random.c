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
	return(CC2520_RANDOM16());
}

uint8 hal_rand_uint8(uint8 n)
{
	return(CC2520_RANDOM8()%n);
}
