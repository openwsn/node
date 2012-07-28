#include "svc_uniqueid.h"

// @todo

void GetLockCode(TiUniqueid *id)
{
	id->CpuID[0]=*(vu32*)(0x1ffff7e8);
	id->CpuID[1]=*(vu32*)(0x1ffff7ec);
	id->CpuID[2]=*(vu32*)(0x1ffff7f0);
}