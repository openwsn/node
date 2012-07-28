
/*
 * This module implements a high precision timer used by the whole system. It keeps
 * the accurate time currently and is capable to generate event when timing expired.
 */

#include "osx_configall.h"
#include "osx_foundation.h"
#include "../rtl/rtl_time.h"
#include "../hal/hal_ticker.h"

#define TiOsxTimer TiSysTimer

/*
#define tm_value_t uint16  

typedef struct{
  uint8 state;
  //uint8 prescale_selector;
  //uint16 prescale;
  tm_value_t interval;
  uint8 reginterval;
  TiFunEventHandler listener;
  void * lisowner;
  uint16 TCCR;
  uint16 OCR3;
}TiSysTimer; // will be upgraded to TiTickerAdapter

TiSysTimer * systm_construct( char * buf, uint8 size );
void systm_destroy( TiSysTimer * timer );
TiSysTimer * systm_open( TiSysTimer * timer, tm_value_t interval, TiFunEventHandler listener, void * lisowner );
void systm_close( TiSysTimer * timer );
void systm_start( TiSysTimer * timer );
void systm_stop( TiSysTimer * timer );
bool systm_expired( TiSysTimer * timer );
*/

TiOsxTimer * _osx_timer_construct( char * buf, uint8 size );


void _osx_timer_destroy( TiOsxTimer * timer );


TiOsxTimer * _osx_timer_open( TiOsxTimer * timer, TiSystemTime interval, TiFunEventHandler listener, void * lisowner );


void _osx_timer_close( TiOsxTimer * timer );

void _osx_timer_start( TiOsxTimer * timer );


void _osx_timer_stop( TiOsxTimer * timer );


int osx_timer_restart( TiOsxTimer * timer, TiSystemTime * interval, uint8 option );

bool _osx_timer_expired( TiOsxTimer * timer );


void clock_forward( TiOsxTimer * timer,TiSystemTime ms);
void clock_backward( TiOsxTimer * timer,TiSystemTime ms);
void clock_set( TiOsxTimer * timer,TiSystemTime ms);
TiSystemTime clock_get( TiOsxTimer * timer);



/*
osx_timer_( TiOsxTimer * timer, TiSystemTime *  )
osx_timer_pause
clcok_resume
clock_forward
clock_backward
clock_adjust
clock_current = clock_snapshot
clock_set/get
clock_disable
clock_enable
*/
