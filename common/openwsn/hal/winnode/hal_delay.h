#ifndef rtl_delayH
#define rtl_delayH
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "hal_configall.h"
#include <time.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "hal_foundation.h"

//---------------------------------------------------------------------------
// Cpu Delay, Process Delay, and UI Nonstop Delay
//---------------------------------------------------------------------------

// us: u seconds
void delay_us(__int64 Us);

// return CPU frequency by MHz
int cpu_frequency(void);
void cpu_delay_us(__int64 Us);


#endif
