#include "hal_types.h"
#include "hal_defs.h"
#include "hal_timer_32k.h"
#include "hal_board.h"
#include "hal_int.h"

void delayxxx()
{
  _NOP();
}

void utilDelay(int delay)
{
  int timer1=0,timer2=0;
  timer1=0;
  while(timer1<320)
  {timer2=0;
   while(timer2<delay)
   {halTimer32kIntEnable();
    halTimer32kInit(32000);
    halTimer32kIntDisable();
    timer2++;}
   timer1++;}
}