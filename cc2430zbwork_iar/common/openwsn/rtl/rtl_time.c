#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "rtl_time.h"


void tm8_reset( uint8 * var, uint8 value ) {*var=value;}
inline void tm8_clear( TiTime8 * var ) {*var=0;}

inline TiTime8 * tm8_plus( TiTime8 * var1, TiTime8 * var2 )
{
	(*var1) += (*var2);
	return var1;
}

inline TiTime8 * tm8_minus( TiTime8 * var1, TiTime8 * var2 ) 
{
	(*var1) -= (*var2);
	return var1;
}

inline bool tm8_forward( TiTime8 * var, uint8 interval )
{
	// return false to indicate overflow during forward
	if (*var + interval < *var)
		return false;
	else
		return true;
}

inline bool tm8_backward( TiTime8 * var, uint8 interval )
{
	// return false to indicate overflow during forward
	if (*var - interval > *var)
		return false;
	else
		return true;
}



inline void tm64_reset( TiTime64 * var, uint64 value ) {*var=value;}
inline void tm64_clear( TiTime64 * var ) {*var=0;}

inline TiTime64 * tm64_plus( TiTime64 * var1, TiTime64 * var2 )
{
	(*var1) += (*var2);
	return var1;
}

inline TiTime64 * tm64_minus( TiTime64 * var1, TiTime64 * var2 ) 
{
	(*var1) -= (*var2);
	return var1;
}

inline bool tm64_forward( TiTime64 * var, uint64 interval )
{
	// return false to indicate overflow during forward
	if (*var + interval < *var)
		return false;
	else
		return true;
}

inline bool tm64_backward( TiTime64 * var, uint64 interval )
{
	// return false to indicate overflow during forward
	if (*var - interval > *var)
		return false;
	else
		return true;
}

/*
should move to rtl_time

Ã·π©time
svc_time is based on rtl_time, hal_timer

rtl_time: provides an data structure to operate the time information
svc_time: provides the device time and system time. The device time is maintained 
by the hardware, which is hardware dependent. The system time is maintained by 
this module, which is hardware independent. Since the system time is usually converted
from the device time, it's often less accurate than the device time, but it's 
convenient to compare with other devices.


typedef TiTime64 TiDeviceTime;
typedef TiTime64 TiSystemTime;

TiDeviceTime g_devtime;
TiSystemTime g_systime;

*/
