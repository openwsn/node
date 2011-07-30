//---------------------------------------------------------------------------
// rtl_delay
//---------------------------------------------------------------------------

#include "hal_configall.h"
#include <tchar.h>
#include <time.h>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "hal_foundation.h"
#include "hal_delay.h"

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

// 高精度延时的程序, 参数: 微秒:
// reference
// Victor Chen, CPU 测速(MHz)和高精度延时(微秒级)
// http://www.cppfans.com/articles/system/cpuspd_usdly.asp

void delay_us(__int64 Us)
{
	LARGE_INTEGER CurrTicks, TicksCount;

	QueryPerformanceFrequency(&TicksCount);
	QueryPerformanceCounter(&CurrTicks);

	// todo 2010.01
	// need revise
	// is it correct?
	TicksCount.QuadPart = TicksCount.QuadPart * Us / 100000064;
	TicksCount.QuadPart += CurrTicks.QuadPart;

	while(CurrTicks.QuadPart<TicksCount.QuadPart)
		QueryPerformanceCounter(&CurrTicks);
}

// reference
// Victor Chen, CPU 测速(MHz)和高精度延时(微秒级)
// http://www.cppfans.com/articles/system/cpuspd_usdly.asp
/*

	 利用 rdtsc 汇编指令可以得到 CPU 内部定时器的值, 每经过一个 CPU 周期, 这个定时器就加一。
如果在一段时间内数得 CPU 的周期数, CPU工作频率 = 周期数 / 时间

为了不让其他进程和线程打扰, 必需要设置最高的优先级
以下函数设置当前进程和线程到最高的优先级。
SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

CPU 测速程序的源代码, 这个程序通过 CPU 在 1/16 秒的时间内经过的周期数计算出工作频率, 单位 MHz:
*/
int cpu_frequency(void) //MHz
{/*
	LARGE_INTEGER CurrTicks, TicksCount;
	__int64 iStartCounter, iStopCounter;

    DWORD dwOldProcessP = GetPriorityClass(GetCurrentProcess());
    DWORD dwOldThreadP = GetThreadPriority(GetCurrentThread());

    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    QueryPerformanceFrequency(&TicksCount);
    QueryPerformanceCounter(&CurrTicks);

    TicksCount.QuadPart /= 16;
    TicksCount.QuadPart += CurrTicks.QuadPart;

    asm rdtsc
    asm mov DWORD PTR iStartCounter, EAX
    asm mov DWORD PTR (iStartCounter+4), EDX

    while(CurrTicks.QuadPart<TicksCount.QuadPart)
        QueryPerformanceCounter(&CurrTicks);

    asm rdtsc
    asm mov DWORD PTR iStopCounter, EAX
    asm mov DWORD PTR (iStopCounter + 4), EDX

    SetThreadPriority(GetCurrentThread(), dwOldThreadP);
    SetPriorityClass(GetCurrentProcess(), dwOldProcessP);

    return (int)((iStopCounter-iStartCounter)/62500);
    */
	// todo
	return 1;
}

// reference
// Victor Chen, CPU 测速(MHz)和高精度延时(微秒级)
// http://www.cppfans.com/articles/system/cpuspd_usdly.asp
// 前面是用 API 函数进行延时, 如果知道了 CPU 的工作频率, 利用循环, 也可以得到高
// 精度的延时

int _CPU_FREQ = 0; //定义一个全局变量保存 CPU 频率 (MHz)

void CpuDelayUs(__int64 Us) //利用循环和 CPU 的频率延时, 参数: 微秒
{
	/*
    __int64 iCounter, iStopCounter;
    asm rdtsc
    asm mov DWORD PTR iCounter, EAX
    asm mov DWORD PTR (iCounter+4), EDX

    iStopCounter = iCounter + Us*_CPU_FREQ;

	while(iStopCounter-iCounter>0)
    {
        asm rdtsc
        asm mov DWORD PTR iCounter, EAX
        asm mov DWORD PTR (iCounter+4), EDX
    }
    */
}

void TestDelay(void)
{
/*
	_CPU_FREQ = CPU_Frequency(); //利用 CPU 频率初始化定时
	CpuDelayUs(1000000); //延时 1 秒钟
	*/
}


