#include "hal_configall.h"
#include "hal_foundation.h"
#include "util_twinkle.h"
#include "hal_led.h"
#include "util_delay.h"

void utiltwinkle(uint8 id,int delay,int count)
{
        int ct;
        ct=0;
    switch (id)
    {
    case 1:  while(ct<count)
            {halLedSet(1);
             utilDelay(delay);
             halLedClear(1);
             utilDelay(delay);
             ct++;} break;
    case 2:  while(ct<count)
            {halLedSet(2);
             utilDelay(delay);
             halLedClear(2);
             utilDelay(delay);
             ct++;}break;
    case 3:  while(ct<count)
            {halLedSet(3);
             utilDelay(delay);
             halLedClear(3);
             utilDelay(delay);
             ct++;} break;
    case 4:  while(ct<count)
            {halLedSet(4);
             utilDelay(delay);
             halLedClear(4);
             utilDelay(delay);
             ct++;} break;
    default: break;
    }
}