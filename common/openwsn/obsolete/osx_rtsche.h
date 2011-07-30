/* an realtime scheduler for osx */

#include "osx_configall.h"
#include "../rtl/rtl_ringqueue.h"
#include "osx_foundation.h"

typedef struct{
	TiRingQueue * fifoqueue;
	TiRingQueue * rtqueue;
	TiFunEventHandler defaulthandler;
}TiSimpleScheduler;
