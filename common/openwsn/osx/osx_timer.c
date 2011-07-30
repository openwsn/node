#include "osx_configall.h"
#include "osx_foundation.h"
#include "../rtl/rtl_time.h"
#include "../hal/hal_ticker.h"
#include "osx_timer.h"

void clock_forward( TiOsxTimer * timer,TiSystemTime ms)
{
	timer->time = timer->time + ms;
}

void clock_backward( TiOsxTimer * timer,TiSystemTime ms)
{
	timer->time = timer->time - ms;
}

void clock_set( TiOsxTimer * timer,TiSystemTime ms)
{
	timer->time = ms;
}

TiSystemTime clock_get( TiOsxTimer * timer)
{
	return timer->time;
}
TiOsxTimer * _osx_timer_construct( char * buf, uint8 size )
{
	return systm_construct( buf, size );
}

void _osx_timer_destroy( TiOsxTimer * timer )
{
	systm_destroy( timer );
}

TiOsxTimer * _osx_timer_open( TiOsxTimer * timer, TiSystemTime interval, TiFunEventHandler listener, void * lisowner )
{
	return systm_open( timer, interval, listener, lisowner );
}

void _osx_timer_close( TiOsxTimer * timer )
{
	systm_close( timer );
}

void _osx_timer_start( TiOsxTimer * timer )
{
	systm_start( timer );
}

void _osx_timer_stop( TiOsxTimer * timer )
{
	//void systm_stop( timer );
}

int osx_timer_restart( TiOsxTimer * timer, TiSystemTime * interval, uint8 option )
{
	return true;
}

bool _osx_timer_expired( TiOsxTimer * timer )
{
	//return systm_expired( timer );
	return true;
}